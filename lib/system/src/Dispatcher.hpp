/*
 * IDispatcher.hpp
 *
 *  Created on: Sep 24, 2017
 *      Author: suoalex
 */

#ifndef LIB_SYSTEM_DISPATCHER_HPP_
#define LIB_SYSTEM_DISPATCHER_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

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
	template<typename Queue> class TaskRunner
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

			thread = new std::thread([] ()
			{
				std::shared_ptr<Runnable> runnable;

				while (!stopRequested)
				{
					if (queue.pop(runnable))
					{
						runnable();
					}
					else if (waitOnEmpty)
					{
						condition.wait();
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
			stopRequested = true;
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

			queue.push_back(runnable);

			// notify where needed - we know there is only 1 thread waiting at most.
			if (wasEmpty && waitOnEmpty)
			{
				condition.notifyOne();
			}
		}

	private:

		// Flag if the thread should wait when queue is empty.
		const bool waitOnEmpty;

		// Flag if stop is called.
		bool stopRequested;

		// Pointer to actual thread.
		std::thread* thread;

		// Condition used for queue notify.
		Condition condition;

		// The queue structure holding the tasks.
		Queue<std::shared_ptr<Runnable> > queue;

	};

	//
	// A hash dispatcher.
	//
	// The tasks are separated into queues based on the hash value of the key submitted.
	//
	template<typename Key, typename Thread, typename Queue> class HashDispatcher
	{
	public:

		// The hash function type.
		typedef std::function<size_t(const Key&) const> HashFunction;

		//
		// Constructor.
		//
		// threadCount : number of threads in the dispatcher.
		// function : The hash function.
		//
		HashDispatcher(size_t threadCount = 4, const HashFunction& function = DEFAULT_HASH) :
			runners(threadCount),
			function(function)
		{
		}

		//
		// Submit a task for the dispatcher to run.
		//
		// key : The key for the task.
		// runnable : The task to be executed.
		//
		void submit(Key key, std::shared_ptr<Runnable>& runnable)
		{
			runners[function(key) % runners.size()].submit(runnable);
		}

	private:

		// The default hash function used.
		static const Poco::Hash<Key> DEFAULT_HASH;

		// The task runners.
		std::vector<TaskRunner> runners;

		// The hash function.
		const HashFunction& function;
	};
}

#endif /* LIB_SYSTEM_DISPATCHER_HPP_ */
