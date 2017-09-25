/*
 * Condition.hpp
 *
 *  Created on: Sep 25, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_CONDITION_HPP_
#define LIB_COMMON_CONDITION_HPP_

#include <mutex>

namespace mm
{
	template<typename Mutex = std::mutex> class Condition
	{
	public:

		//
		// Constructor.
		//
		Condition() : lock(mutex)
		{
		}

		//
		// Wait till the condition is triggered.
		//
		inline void wait()
		{
			cv.wait(lock);
		}

		//
		// Wait till the condition is triggered to execute the predicate.
		//
		// predicate : The function to call after wait is done.
		//
		template<class Predicate> inline void wait(Predicate predicate)
		{
			cv.wait(lock, predicate);
		}

		//
		// Notify one waiting thread.
		//
		inline void notifyOne()
		{
			cv.notify_one();
		}

		//
		// Notify all waiting threads.
		//
		inline void notifyAll()
		{
			cv.notify_all();
		}

	private:

		// The mutex used by the lock.
		Mutex mutex;

		// The lock used by the condition.
		std::unique_lock<Lock> lock;

		// The condition variable itself.
		std::condition_variable cv;
	};
}

#endif /* LIB_COMMON_CONDITION_HPP_ */
