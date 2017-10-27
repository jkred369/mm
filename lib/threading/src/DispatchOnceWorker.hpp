/*
 * DIspatchOnceWorker.hpp
 *
 *  Created on: Oct 27, 2017
 *      Author: suoalex
 */

#ifndef LIB_THREADING_SRC_DISPATCHONCEWORKER_HPP_
#define LIB_THREADING_SRC_DISPATCHONCEWORKER_HPP_

#include <atomic>
#include <cstdint>
#include <exception>

#include "Runnable.hpp"

namespace mm
{
	//
	// The task wrapper.
	//
	template<typename Dispatcher, typename Key = std::int32_t> class RunnableWrapper
	{
	public:

		//
		// Constructor.
		//
		RunnableWrapper(
				const Key& key,
				Runnable& task,
				Dispatcher& dispatcher,
				std::atomic<std::int32_t>& counter,
				Runnable& rescheduleTask) :
					key(key),
					task(task),
					dispatcher(dispatcher),
					counter(counter),
					rescheduleTask(rescheduleTask)
		{
			counter.store(1);
		}

		//
		// Destructor. Reschedules where appropriate.
		//
		~RunnableWrapper()
		{
			if (counter.fetch_sub(1) > 1)
			{
				dispatcher.submit(key, rescheduleTask);
			}
		}

		//
		// The calling operator.
		//
		void operator() ()
		{
			try
			{
				task();
			}
			catch (...)
			{
			}
		}

	private:

		// The key for the task.
		const Key& key;

		// The actual task.
		Runnable& task;

		// The dispatcher.
		Dispatcher& dispatcher;

		// The counter used.
		std::atomic<std::int32_t>& counter;

		// The task for reschedule.
		Runnable& rescheduleTask;
	};

	//
	// The worker class so that the inner task will only be executed once.
	//
	template<typename Dispatcher, typename Key = std::int32_t> class DispatchOnceWorker
	{
	public:

		//
		// Constructor.
		//
		// key : Key for the task.
		// runnable : The actual task.
		// dispatcher : The dispatcher instance.
		//
		DispatchOnceWorker(const Key& key, Runnable& task, Dispatcher& dispatcher)
			: key(key), task(task), dispatcher(dispatcher)
		{
			runnable = [this] ()
			{
				RunnableWrapper<Dispatcher, Key>(this->key, this->task, this->dispatcher, counter, runnable)();
			};
		}

		//
		// Dispatch the work.
		//
		void dispatch()
		{
			if (counter.fetch_add(1) == 0)
			{
				dispatcher.submit(key, runnable);
			}
		}

	private:

		// The dispatch counter.
		std::atomic<std::int32_t> counter;

		// Key of the task.
		const Key key;

		// The actual task.
		Runnable task;

		// The dispatcher used.
		Dispatcher& dispatcher;

		// The task to dispatch.
		Runnable runnable;
	};
}

#endif /* LIB_THREADING_SRC_DISPATCHONCEWORKER_HPP_ */
