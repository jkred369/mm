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
	template<typename Key = std::int32_t, typename Dispatcher> class RunnableWrapper
	{
	public:

		//
		// Constructor.
		//
		RunnableWrapper(
				const Key& key,
				Runnable& runnable,
				Dispatcher& dispatcher,
				atomic<std::int32_t>& counter) :
					key(key),
					runnable(runnable),
					dispatcher(dispatcher),
					counter(counter)
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
				dispatcher.dispatch(RunnableWrapper(key, runnable, dispatcher, counter));
			}
		}

		//
		// The calling operator.
		//
		void operator() ()
		{
			runnable();
		}

	private:

		// The key for the task.
		const Key& key;

		// The actual task.
		Runnable& runnable;

		// The dispatcher.
		Dispatcher& dispatcher;

		// The counter used.
		atomic<std::int32_t>& counter;
	}

	//
	// The worker class so that the inner task will only be executed once.
	//
	template<typename Key = std::int32_t, typename Dispatcher> class DispatchOnceWorker
	{
	public:

		//
		// Constructor.
		//
		// key : Key for the task.
		// runnable : The actual task.
		// dispatcher : The dispatcher instance.
		//
		DispatchOnceWorker(const Key& key, Runnable& runnable, Dispatcher& dispatcher)
			: key(key), dispatcher(dispatcher)
		{
			this->runnable = [] ()
			{
				RunnableWrapper(this->key, runnable, this->dispatcher, counter)();
			};
		}

		//
		// Dispatch the work.
		//
		void dispatch()
		{
			if (counter.fetch_add(1) == 0)
			{
				dispatcher.dispatch(runnable);
			}
		}

	private:

		// Key of the task.
		const Key key;

		// The task to dispatch.
		Runnable runnable;

		// The dispatcher used.
		Dispatcher& dispatcher;

		// The dispatch counter.
		atomic<std::uint32_t> counter;
	};
}

#endif /* LIB_THREADING_SRC_DISPATCHONCEWORKER_HPP_ */
