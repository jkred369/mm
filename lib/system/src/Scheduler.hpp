/*
 * Scheduler.hpp
 *
 *  Created on: Oct 11, 2017
 *      Author: suoalex
 */

#ifndef LIB_SYSTEM_SRC_SCHEDULER_HPP_
#define LIB_SYSTEM_SRC_SCHEDULER_HPP_

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>

#include <tbb/concurrent_priority_queue.h>

#include <Timer.hpp>
#include "Dispatcher.hpp"

namespace mm
{
	//
	// The struct for a delayed runnable.
	//
	struct DelayedRunnable
	{
		// The timestamp as number of nanoseconds from epoch.
		std::int64_t timestampInNanos;

		// The runnable
		Runnable runnable;

		//
		// Overload the operator for comparison.
		//
		// lhs : First argument.
		// rhs : Second argument.
		//
		// return : true if timestampInNanos in lhs less than that in rhs.
		//
		bool operator < (const DelayedRunnable& lhs, const DelayedRunnable& rhs) const
		{
			return lhs.timestampInNanos < rhs.timestampInNanos;
		}
	};

	//
	// The scheduler working together with dispatcher.
	//
	template<typename Key = std::int32_t, typename Timer = HighResTimer, typename Dispatcher> class Scheduler
	{
	public:

		//
		// Constructor.
		//
		// dispatcher : The dispatcher which will execute the actual task.
		//
		Scheduler(Dispatcher& dispatcher) : dispatcher(dispatcher), stopRequested(false), scheduleThread(run)
		{
		}

	protected:

		//
		// Execute the tasks from the queue.
		//
		void run()
		{
			bool hasTask = false;
			DelayedRunnable runnable;

			while (!stopRequested.load())
			{
				// make sure the runnable is the next to execute
				if (hasTask)
				{
					if (nextTaskTimestamp.load() < runnable.timestampInNanos)
					{
						// we know the try pop will at least retrieve 1 item
						queue.push(runnable);
						queue.try_pop(runnable);
					}
				}
				else
				{
					hasTask = queue.try_pop(runnable);
				}

				// deal with the task or wait
				if (hasTask)
				{
					std::int64_t periodInNanos = Timer.getTimeInNanos() - runnable.timestampInNanos;

					if (periodInNanos <= 0)
					{
						dispatcher.submit(runnable.runnable());
					}
					else
					{
						nextTaskTimestamp.store(runnable.timestampInNanos);

						std::unique_lock<std::recursive_timed_mutex> lock(mutex);
						condition.wait_for(lock, std::chrono::nanoseconds(periodInNanos));
					}
				}
				else
				{
					// enter inactive thread wait for next task
					std::unique_lock<std::recursive_timed_mutex> lock(mutex);
					while (queue.empty() && !stopRequested.load())
					{
						condition.wait(lock);
					}
				}
			}
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The delay in nanoseconds.
		//
		void schedule(const Key key, const Runnable& runnable, std::int64_t delay)
		{
			scheduleAt(key, runnable, Timer.getTimeInNanos() + delay);
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// timestamp : The time to execute the task.
		//
		void scheduleAt(const Key key, const Runnable& runnable, std::int64_t timestamp)
		{
			bool wasEmpty = queue.empty();
			queue.push({timestamp, runnable});

			bool queueJumped = timestamp < nextTaskTimestamp.load();
			if (queueJumped)
			{
				nextTaskTimestamp.store(timestamp);
			}

			if (wasEmpty || queueJumped)
			{
				std::lock_guard<std::recursive_timed_mutex> guard(mutex);
				condition.notify_all();
			}
		}

		//
		// Schedule a task at fixed rate.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The initial delay in nanoseconds.
		// interval : The interval in nanoseconds between 2 task start time.
		//
		void scheduleAtFixedRate(const Key key, const Runnable& runnable, std::int64_t delay, std::int64_t interval)
		{
			Runnable recursiveRunnable = [this, runnable, interval]()
			{
				std::int64_t startTime = Timer.getTimeInNanos();
				runnable();

				this->scheduleAt(recursiveRunnable, startTime + interval);
			};

			scheduleAt(key, recursiveRunnable, Timer.getTimeInNanos() + delay);
		}

		//
		// Schedule a task with fixed delay.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The initial delay in nanoseconds.
		// interval : The interval in nanoseconds between task end and next task start.
		//
		void scheduleWithFixedDelay(const Key key, const Runnable& runnable, std::int64_t delay, std::int64_t interval)
		{
			Runnable recursiveRunnable = [this, runnable, interval]()
			{
				runnable();
				this->scheduleAt(recursiveRunnable, Timer.getTimeInNanos() + interval);
			};

			scheduleAt(recursiveRunnable, Timer.getTimeInNanos() + delay);
		}

	private:

		// The dispatcher used.
		Dispatcher& dispatcher;

		// The timer used for scheduling.
		Timer timer;

		// The next task to execute with the smallest timestamp (i.e. fastest execution).
		atomic<std::int64_t> nextTaskTimestamp;

		// The priority queue for the tasks.
		tbb::concurrent_priority_queue<DelayedRunnable> queue;

		// The mutex for scheduling.
		std::recursive_timed_mutex mutex;

		// The condition variable used.
		std::condition_variable condition;

		// Flag if stop is requested
		std::atomic<bool> stopRequested;

		// The schedule thread.
		std::thread scheduleThread;
	};
}



#endif /* LIB_SYSTEM_SRC_SCHEDULER_HPP_ */
