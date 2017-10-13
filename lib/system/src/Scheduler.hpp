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
			bool awaiting = false;
			DelayedRunnable runnable;

			while (!stopRequested.load())
			{
				if (awaiting)
				{
					queue.push(runnable);
					awaiting = false;
				}

				if (queue.try_pop(runnable))
				{
					std::int64_t periodInNanos = Timer.getTimeInNanos() - runnable.timestampInNanos;
					if (periodInNanos <= 0)
					{
						runnable.runnable();
					}
					else
					{
						awaiting = true;
					}
				}
				else
				{
					// enter inactive thread wait
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
		// runnable : The task to schedule.
		// delay : The delay in nanoseconds.
		//
		void schedule(const Runnable& runnable, std::int64_t delay)
		{
			scheduleAt(runnable, Timer.getTimeInNanos() + delay);
		}

		//
		// Schedule a given task.
		//
		// runnable : The task to schedule.
		// timestamp : The time to execute the task.
		//
		void scheduleAt(const Runnable& runnable, std::int64_t timestamp)
		{
			bool wasEmpty = queue.empty();
			queue.push({timestamp, runnable});

			if (wasEmpty)
			{
				std::lock_guard<std::recursive_timed_mutex> guard(mutex);
				condition.notify_all();
			}
		}

		//
		// Schedule a task at fixed rate.
		//
		// runnable : The task to schedule.
		// delay : The initial delay in nanoseconds.
		// interval : The interval in nanoseconds between 2 task start time.
		//
		void scheduleAtFixedRate(const Runnable& runnable, std::int64_t delay, std::int64_t interval)
		{
			Runnable recursiveRunnable = [this, runnable, interval]()
			{
				std::int64_t startTime = Timer.getTimeInNanos();
				runnable();

				this->scheduleAt(recursiveRunnable, startTime + interval);
			};

			scheduleAt(recursiveRunnable, Timer.getTimeInNanos() + delay);
		}

		//
		// Schedule a task with fixed delay.
		//
		// runnable : The task to schedule.
		// delay : The initial delay in nanoseconds.
		// interval : The interval in nanoseconds between task end and next task start.
		//
		void scheduleWithFixedDelay(const Runnable& runnable, std::int64_t delay, std::int64_t interval)
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

		// The next task in the queue with the smallest timestamp (i.e. fastest execution).
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
