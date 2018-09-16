/*
 * ScheduleOnceWorker.hpp
 *
 *  Created on: Oct 28, 2017
 *      Author: suoalex
 */

#ifndef LIB_THREADING_SRC_SCHEDULEONCEWORKER_HPP_
#define LIB_THREADING_SRC_SCHEDULEONCEWORKER_HPP_

#include <atomic>
#include <chrono>
#include <cstdint>
#include <exception>

#include <TimeUtil.hpp>

#include "Runnable.hpp"

namespace mm
{
	//
	// The worker class so that the inner task will only be executed once.
	//
	template<typename Scheduler, typename Key = std::int32_t> class ScheduleOnceWorker
	{
	public:

		//
		// Constructor.
		//
		// key : Key for the task.
		// runnable : The actual task.
		// delay : The delay as duration.
		// scheduler : The scheduler instance.
		//
		template <typename Rep, typename Period> ScheduleOnceWorker(
				const Key& key,
				Runnable& task,
				const std::chrono::duration<Rep, Period>& delay,
				Scheduler& scheduler) :
			key(key),
			task(task),
			delayInNanos(durationToNanos(delay)),
			scheduler(scheduler),
			counter(0)
		{
		}

		//
		// Constructor.
		//
		// key : Key for the task.
		// runnable : The actual task.
		// delayInNanos : The delay in nanoseconds.
		// scheduler : The scheduler instance.
		//
		ScheduleOnceWorker(const Key& key, Runnable& task, std::int64_t delayInNanos, Scheduler& scheduler)
			: key(key), task(task), delayInNanos(delayInNanos), scheduler(scheduler), counter(0)
		{
			runnable = [this] ()
			{
				RunnableWrapper(*this)();
			};
		}

		//
		// Schedule the work.
		//
		void schedule()
		{
			if (counter.fetch_add(1) == 0)
			{
				scheduler.schedule(key, runnable, delayInNanos);
			}
		}

	protected:

		//
		// The wrapper which does the job on dispatcher. Implemented for RAII.
		//
		//
		//
		class RunnableWrapper
		{
		public:

			//
			// Constructor.
			//
			// worker : The worker used.
			//
			RunnableWrapper(ScheduleOnceWorker<Scheduler, Key>& worker) : worker(worker)
			{
				worker.counter.store(1);
			}

			//
			// Destructor. Reschedules where appropriate.
			//
			~RunnableWrapper()
			{
				if (worker.counter.fetch_sub(1) > 1)
				{
					worker.scheduler.schedule(worker.key, worker.runnable, worker.delayInNanos);
				}
			}

			//
			// The calling operator.
			//
			void operator() ()
			{
				try
				{
					worker.task();
				}
				catch (...)
				{
				}
			}

		private:

			// The worker used.
			ScheduleOnceWorker<Scheduler, Key>& worker;
		};

		friend class RunnableWrapper;

	private:

		// Key of the task.
		const Key key;

		// The actual task.
		Runnable task;

		// The delay in nanoseconds.
		const std::int64_t delayInNanos;

		// The dispatcher used.
		Scheduler& scheduler;

		// The dispatch counter.
		std::atomic<std::int32_t> counter;

		// The task to dispatch.
		Runnable runnable;
	};
}

#endif /* LIB_THREADING_SRC_SCHEDULEONCEWORKER_HPP_ */
