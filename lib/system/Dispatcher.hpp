/*
 * IDispatcher.hpp
 *
 *  Created on: Sep 24, 2017
 *      Author: suoalex
 */

#ifndef LIB_SYSTEM_DISPATCHER_HPP_
#define LIB_SYSTEM_DISPATCHER_HPP_

#include <functional>
#include <memory>

namespace mm
{
	typedef std::function<void()> Runnable;

	//
	// A hash dispatcher.
	//
	// The tasks are separated into queues based on the hash value of the key submitted.
	//
	template<typename Key, typename HashFunction, typename Thread, typename Queue> class HashDispatcher
	{
	public:

		//
		// Submit a task for the dispatcher to run.
		//
		// key : The key for the task.
		// runnable : The task to be executed.
		//
		void submit(Key key, std::shared_ptr<Runnable> runnable)
		{

		}

	private:
	};
}

#endif /* LIB_SYSTEM_DISPATCHER_HPP_ */
