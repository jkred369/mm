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
#include <iostream>

#include "Runnable.hpp"

namespace mm
{
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
			: key(key), task(task), dispatcher(dispatcher), counter(0)
		{
			runnable = [this] ()
			{
				RunnableWrapper(*this)();
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
			RunnableWrapper(DispatchOnceWorker<Dispatcher, Key>& worker) : worker(worker)
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
					worker.dispatcher.submit(worker.key, worker.runnable);
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
			DispatchOnceWorker<Dispatcher, Key>& worker;
		};

		friend class RunnableWrapper;

	private:

		// Key of the task.
		const Key key;

		// The actual task.
		Runnable task;

		// The dispatcher used.
		Dispatcher& dispatcher;

		// The dispatch counter.
		std::atomic<std::int32_t> counter;

		// The task to dispatch.
		Runnable runnable;
	};
}

#endif /* LIB_THREADING_SRC_DISPATCHONCEWORKER_HPP_ */
