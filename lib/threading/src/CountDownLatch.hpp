/*
 * Condition.hpp
 *
 *  Created on: Sep 25, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_CONDITION_HPP_
#define LIB_COMMON_CONDITION_HPP_

#include <cstdint>
#include <condition_variable>
#include <mutex>

namespace mm
{
	//
	// Implement a count down latch.
	//
	template<typename Mutex = std::mutex> class CountDownLatch
	{
	public:

		//
		// Constructor.
		//
		// count : The total count.
		//
		CountDownLatch(std::uint32_t count) : count(count)
		{
		}

		//
		// Wait till the count reaches 0.
		//
		inline void wait()
		{
			std::unique_lock<Mutex> lock(mutex);
			while (count != 0)
			{
				cv.wait(lock);
			}
		}

		//
		// Count down the latch.
		//
		inline void countDown()
		{
			std::lock_guard<std::mutex> guard(mutex);
			if (count > 0)
			{
				--count;
			}

			if (count == 0)
			{
				cv.notify_all();
			}
		}

	private:

		// The mutex used by the lock.
		Mutex mutex;

		// The count of the count down latch.
		std::uint32_t count;

		// The condition variable itself.
		std::condition_variable cv;
	};
}

#endif /* LIB_COMMON_CONDITION_HPP_ */
