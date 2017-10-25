/*
 * SpinLockGuard.hpp
 *
 *  Created on: Oct 25, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_SPINLOCKGUARD_HPP_
#define LIB_COMMON_SRC_SPINLOCKGUARD_HPP_

#include <mutex>

#include "Timer.hpp"

namespace mm
{
	//
	// A spin lock guard which firstly tyies to spin for a while and then lock in usual pattern.
	//
	template<typename Mutex = std::mutex, typename Timer = HighResTimer> class SpinLockGuard
	{
	public:

		//
		// Constructor. Locks the given mutex.
		//
		// mutex : The mutex instance.
		// spinTimeInNanos : The spin time in nanoseconds.
		//
		//
		SpinLockGuard(Mutex& mutex, std::int64_t spinTimeInNanos = 10000) : mutex(mutex)
		{
			if (!mutex.try_lock())
			{
				const Timer timer;
				const std::int64_t startTime = timer.getTimeInNanos();

				for (std::int64_t endTime = startTime;
						endTime - startTime < spinTimeInNanos;
						endTime = timer.getTimeInNanos())
				{
					if (mutex.try_lock())
					{
						return;
					}
				}

				// enter blocking lock
				mutex.lock();
			}
		}

		//
		// Destructor - unlock the mutex.
		//
		~SpinLockGuard()
		{
			mutex.unlock();
		}

	private:

		// The mutex used.
		Mutex& mutex;
	};
}



#endif /* LIB_COMMON_SRC_SPINLOCKGUARD_HPP_ */
