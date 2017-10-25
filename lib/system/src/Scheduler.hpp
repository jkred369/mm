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
#include <iostream>
#include <mutex>
#include <thread>

#include <tbb/concurrent_priority_queue.h>

#include <Timer.hpp>
#include <TimeUtil.hpp>

#include "Dispatcher.hpp"

namespace mm
{
	//
	// The struct for a delayed runnable.
	//
	template<typename Key> struct DelayedRunnable
	{
		// The key for the runnable to be dispatched.
		Key key;

		// The runnable
		Runnable runnable;

		// The timestamp as number of nanoseconds from epoch.
		std::int64_t timestampInNanos;

		// The repeatitive period in nanoseconds.
		std::int64_t periodInNanos;

		//
		// Overload the operator for comparison.
		//
		// rhs : Comparand.
		//
		// return : true if timestampInNanos in lhs less than that in rhs.
		//
		bool operator < (const DelayedRunnable& rhs) const
		{
			return timestampInNanos > rhs.timestampInNanos;
		}
	};

	//
	// The scheduler working together with dispatcher.
	//
	template<typename Dispatcher, typename Key = std::int32_t, typename Timer = HighResTimer> class DefaultScheduler
	{
	public:

		//
		// Constructor.
		//
		// dispatcher : The dispatcher which will execute the actual task.
		//
		DefaultScheduler(Dispatcher& dispatcher) :
			dispatcher(dispatcher),
			stopRequested(false),
			scheduleThread([this] () { this->run(); } )
		{
		}

		//
		// Destructor. Stop the schedule thread.
		//
		~DefaultScheduler()
		{
			if (!stopRequested.load() || scheduleThread.joinable())
			{
				stopRequested.store(true);

				// notify then wait for thread
				{
					std::lock_guard<std::recursive_mutex> guard(mutex);
					condition.notify_all();
				}

				scheduleThread.join();
			}
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The delay as duration object.
		//
		template <typename Rep, typename Period> inline void schedule(
				const Key key,
				const Runnable& runnable,
				const std::chrono::duration<Rep, Period>& delay)
		{
			schedule(key, runnable, durationToNanos(delay));
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
			scheduleAt(key, runnable, timer.getTimeInNanos() + delay);
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// timestamp : The time to execute the task as time point.
		//
		template<typename Clock, typename Duration> inline void scheduleAt(
				const Key key,
				const Runnable& runnable,
				const std::chrono::time_point<Clock, Duration>& timestamp)
		{
			scheduleAt(key, runnable, timePointToEpochNanos(timestamp));
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// timestamp : The time to execute the task.
		// period : The repeat period in nanoseconds.
		//
		void scheduleAt(const Key key, const Runnable& runnable, std::int64_t timestamp, std::int64_t period = 0)
		{
			if (stopRequested.load())
			{
				return;
			}

			queue.push({key, runnable, timestamp, period});

			// we always notify here so the scheduler thread will re-check the queue.
			{
				std::lock_guard<std::recursive_mutex> guard(mutex);
				condition.notify_all();
			}
		}

		//
		// Schedule a task at fixed rate.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The initial delay as duration object.
		// interval : The interval as duration object between 2 task start time.
		//
		template <typename Rep, typename Period> inline void scheduleAtFixedRate(
				const Key key,
				const Runnable& runnable,
				const std::chrono::duration<Rep, Period>& delay,
				const std::chrono::duration<Rep, Period>& interval)
		{
			scheduleAtFixedRate(key, runnable,
					durationToNanos(delay),
					durationToNanos(interval));
		}

		//
		// Schedule a task at fixed rate.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The initial delay in nanoseconds.
		// interval : The interval in nanoseconds between 2 task start time.
		//
		void scheduleAtFixedRate(
				const Key key,
				const Runnable& runnable,
				std::int64_t delay,
				std::int64_t interval)
		{
			scheduleAt(key, runnable, timer.getTimeInNanos() + delay, interval);
		}

	protected:

		//
		// Execute the tasks from the queue.
		//
		void run()
		{
			bool hasTask = false;
			std::cv_status status = std::cv_status::no_timeout;
			DelayedRunnable<Key> runnable;

			std::unique_lock<std::recursive_mutex> lock(mutex);

			while (!stopRequested.load())
			{
				// make sure the runnable is the next to execute
				if (hasTask)
				{
					queue.push(runnable);
				}
				hasTask = queue.try_pop(runnable);

				// deal with the task or wait
				if (hasTask)
				{
					std::int64_t periodInNanos = runnable.timestampInNanos - timer.getTimeInNanos();

					if (periodInNanos <= 0)
					{
						dispatcher.submit(runnable.key, runnable.runnable);
						hasTask = false;

						if (runnable.periodInNanos != 0)
						{
							queue.push({
								runnable.key,
								runnable.runnable,
								runnable.timestampInNanos + runnable.periodInNanos,
								runnable.periodInNanos});
						}
					}
					else
					{
						status = condition.wait_for(lock, std::chrono::nanoseconds(periodInNanos));

						if (status == std::cv_status::timeout)
						{
							dispatcher.submit(runnable.key, runnable.runnable);
							hasTask = false;

							if (runnable.periodInNanos != 0)
							{
								queue.push({
									runnable.key,
									runnable.runnable,
									runnable.timestampInNanos + runnable.periodInNanos,
									runnable.periodInNanos});
							}
						}
					}
				}
				else
				{
					// enter inactive thread wait for next task
					while (queue.empty() && !stopRequested.load())
					{
						condition.wait(lock);
					}
				}
			}
		}

	private:

		// The dispatcher used.
		Dispatcher& dispatcher;

		// The timer used for scheduling.
		Timer timer;

		// The priority queue for the tasks.
		tbb::concurrent_priority_queue<DelayedRunnable<Key> > queue;

		// The mutex for scheduling.
		std::recursive_mutex mutex;

		// The condition variable used.
		std::condition_variable_any condition;

		// Flag if stop is requested
		std::atomic<bool> stopRequested;

		// The schedule thread.
		std::thread scheduleThread;
	};
}



#endif /* LIB_SYSTEM_SRC_SCHEDULER_HPP_ */
