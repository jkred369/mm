/*
 * IDispatcher.hpp
 *
 *  Created on: Sep 24, 2017
 *      Author: suoalex
 */

#ifndef LIB_SYSTEM_DISPATCHER_HPP_
#define LIB_SYSTEM_DISPATCHER_HPP_

#include <atomic>
#include <cstdint>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <tbb/concurrent_queue.h>

#include <Poco/Hash.h>

namespace mm
{
	typedef std::function<void()> Runnable;

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
		TaskRunner(bool waitOnEmpty = true) : waitOnEmpty(waitOnEmpty), stopRequested(false), thread(nullptr)
		{
		}

		//
		// Destructor.
		//
		~TaskRunner()
		{
			if (thread != nullptr)
			{
				delete thread;
			}
		}

		//
		// Start execution of the tasks.
		//
		void start()
		{
			// sanity check
			if (thread != nullptr)
			{
				return;
			}

			thread = new std::thread([this] ()
			{
				std::shared_ptr<Runnable> runnable;

				while (!stopRequested.load())
				{
					if (queue.try_pop(runnable))
					{
						(*runnable)();
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
			});
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
				std::lock_guard<Mutex> guard(mutex);
				condition.notify_all();
			}

			thread->join();
		}

		//
		// Submit a task into the runner.
		//
		// runnable : The task to be executed.
		//
		void submit(std::shared_ptr<Runnable>& runnable)
		{
			bool wasEmpty = waitOnEmpty ? queue.empty() : false;

			queue.push(runnable);

			// notify where needed
			if (wasEmpty && waitOnEmpty)
			{
				std::lock_guard<Mutex> guard(mutex);
				condition.notify_all();
			}
		}

	private:

		// Flag if the thread should wait when queue is empty.
		const bool waitOnEmpty;

		// Flag if stop is called.
		std::atomic<bool> stopRequested;

		// Pointer to actual thread.
		std::thread* thread;

		// Mutex used for queue notify.
		Mutex mutex;

		// The condition variable used.
		std::condition_variable condition;

		// The queue structure holding the tasks.
		tbb::concurrent_queue<std::shared_ptr<Runnable> > queue;

	};

	//
	// A hash dispatcher.
	//
	// The tasks are separated into queues based on the hash value of the key submitted.
	//
	template<
		typename Key = std::int32_t,
		typename Mutex = std::mutex,
		typename Hash = std::hash<Key>
	> class HashDispatcher
	{
	public:

		//
		// Constructor.
		//
		// threadCount : number of threads in the dispatcher.
		//
		HashDispatcher(size_t threadCount = 4) : runners(threadCount)
		{
			for (TaskRunner<Mutex>& runner : runners)
			{
				runner.start();
			}
		}

		//
		// Destructor. Will stop all the execution internally.
		//
		~HashDispatcher()
		{
			for (TaskRunner<Mutex>& runner : runners)
			{
				runner.stop();
			}
		}

		//
		// Submit a task for the dispatcher to run.
		//
		// key : The key for the task.
		// runnable : The task to be executed.
		//
		void submit(Key& key, std::shared_ptr<Runnable>& runnable)
		{
			runners[hash(key) % runners.size()].submit(runnable);
		}

	private:

		// The hash function / object used.
		Hash hash;

		// The task runners.
		std::vector<TaskRunner<Mutex> > runners;
	};
}

#endif /* LIB_SYSTEM_DISPATCHER_HPP_ */
