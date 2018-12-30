/*
 * IDispatcher.hpp
 *
 *  Created on: Sep 24, 2017
 *      Author: suoalex
 */

#ifndef LIB_THREADING_DISPATCHER_HPP_
#define LIB_THREADING_DISPATCHER_HPP_

#include <atomic>
#include <cstdint>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <tbb/concurrent_queue.h>

#include <Poco/Hash.h>

#include <Logger.hpp>
#include <ThreadUtil.hpp>

#include "Runnable.hpp"
#include "DispatchKey.hpp"

namespace mm
{
	//
	// The dispatcher config items.
	//
	struct DispatcherConfig
	{
		// The dispatcher config
		static const std::string DISPATCHER_CONFIG;

		// The thread count.
		static const std::string THREAD_COUNT;

		// Flag if to wait on queue empty
		static const std::string WAIT_ON_EMPTY;

		// The CPU affinity for threads.
		static const std::string CPU_AFFINITY;
	};

	//
	// The basic execution unit of a hash dispatcher, this class is simply
	// a thread taking all tasks from a given queue.
	//
	// The queue is expected to be thread-safe and the thread will execute tasks from the queue.
	//
	template<typename Mutex = std::mutex> class TaskRunner
	{
	public:

		//
		// Constructor.
		//
		// waitOnEmpty : Flag if thread should enter passive wait when queue is empty.
		//
		TaskRunner(bool waitOnEmpty = true, int cpuId = ThreadUtil::CPU_ID_NOT_SET) :
			waitOnEmpty(waitOnEmpty), cpuId(cpuId), stopRequested(false), thread(nullptr)
		{
		}

		// Disable copy constructor.
		TaskRunner(const TaskRunner&) = delete;

		//
		// Destructor.
		//
		~TaskRunner()
		{
		}

		//
		// Start execution of the tasks.
		//
		bool start()
		{
			// sanity check
			if (thread.get() != nullptr)
			{
				return false;
			}

			thread.reset(new std::thread([this] ()
			{
				// set the CPU affinity where needed
				if (cpuId != ThreadUtil::CPU_ID_NOT_SET)
				{
					if (!ThreadUtil::setAffinity(cpuId))
					{
						LOGFATAL("Thread pin failed to core {}", cpuId);
					}

					LOGINFO("Thread pinned to core {}", cpuId);
				}

				// start the queue
				Runnable runnable;
				while (!stopRequested.load())
				{
					if (queue.try_pop(runnable))
					{
						runnable();
					}
					else if (waitOnEmpty)
					{
						// enter inactive thread wait
						std::unique_lock<Mutex> lock(mutex);
						while (queue.empty() && !stopRequested.load())
						{
							condition.wait(lock);
						}
					}
				}
			}));

			return true;
		}

		//
		// Stop the runner and optionally wait for the tasks to finish.
		//
		// waitForDone : Flag if the thread should be stopped immediately.
		//
		void stop()
		{
			stopRequested.store(true);

			// signal the thread to check flag if needed
			if (waitOnEmpty && queue.empty())
			{
				condition.notify_all();
			}

			if (thread)
			{
				thread->join();
			}
		}

		//
		// Submit a task into the runner.
		//
		// runnable : The task to be executed.
		//
		void submit(const Runnable& runnable)
		{
			bool wasEmpty = waitOnEmpty ? queue.empty() : false;

			queue.push(runnable);

			// notify where needed
			if (wasEmpty && waitOnEmpty)
			{
				condition.notify_all();
			}
		}

	private:

		// Logger of the class.
		static Logger logger;

		// Flag if the thread should wait when queue is empty.
		const bool waitOnEmpty;

		// The CPU ID if an affinity is set.
		const int cpuId;

		// Flag if stop is called.
		std::atomic<bool> stopRequested;

		// Pointer to actual thread.
		std::unique_ptr<std::thread> thread;

		// Mutex used for queue notify.
		Mutex mutex;

		// The condition variable used.
		std::condition_variable condition;

		// The queue structure holding the tasks.
		tbb::concurrent_queue<Runnable> queue;

	};

	//
	// A hash dispatcher.
	//
	// The tasks are separated into queues based on the hash value of the key submitted.
	//
	template<
		typename Key = KeyType,
		typename Mutex = std::mutex,
		typename Hash = std::hash<Key>
	> class HashDispatcher
	{
	public:

		//
		// Constructor.
		//
		// threadCount : number of threads in the dispatcher.
		// startOnCreate : Flag if to start at creation.
		// waitOnEmpty : Flag if the task runners should wait when queue is empty.
		// cpuIds : The CPU IDS to set affinity, if any
		//
		HashDispatcher(
				size_t threadCount = 4,
				bool startOnCreate = true,
				bool waitOnEmpty = true,
				std::vector<int> cpuIds = {}) : runners(threadCount), runningFlag(false)
		{
			for (std::size_t i = 0; i < threadCount; ++i)
			{
				runners[i].reset(new TaskRunner<Mutex> (waitOnEmpty, cpuIds.empty() ? ThreadUtil::CPU_ID_NOT_SET : cpuIds[i]));
			}

			if (startOnCreate)
			{
				start();
			}
		}

		//
		// Destructor. Will stop all the execution internally.
		//
		~HashDispatcher()
		{
			stop();
		}

		//
		// Start the dispatcher.
		//
		bool start()
		{
			bool expected = false;

			if (runningFlag.compare_exchange_strong(expected, true))
			{
				for (std::shared_ptr<TaskRunner<Mutex> >& runner : runners)
				{
					if (!runner->start())
					{
						return false;
					}
				}
			}

			return true;
		}

		//
		// Stop the dispatcher.
		//
		void stop()
		{
			bool expected = true;

			if (runningFlag.compare_exchange_strong(expected, false))
			{
				for (std::shared_ptr<TaskRunner<Mutex> >& runner : runners)
				{
					if (runner)
					{
						runner->stop();
					}
				}
			}
		}

		//
		// Submit a task for the dispatcher to run.
		//
		// key : The key for the task.
		// runnable : The task to be executed.
		//
		void submit(const Key& key, const Runnable& runnable)
		{
			runners[hash(key) % runners.size()]->submit(runnable);
		}

	private:

		// The hash function / object used.
		Hash hash;

		// The task runners.
		std::vector<std::shared_ptr<TaskRunner<Mutex> > > runners;

		// Flag if the dispatcher is running.
		std::atomic<bool> runningFlag;
	};

	// Define the globally used dispatcher type.
	typedef HashDispatcher<> Dispatcher;
}

template<typename Mutex> mm::Logger mm::TaskRunner<Mutex>::logger;

#endif /* LIB_THREADING_DISPATCHER_HPP_ */
