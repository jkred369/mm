/*
 * Timer.hpp
 *
 *  Created on: Oct 13, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_TIMER_HPP_
#define LIB_COMMON_SRC_TIMER_HPP_

#include <chrono>
#include <cstdint>

namespace mm
{
	//
	// Define timers with the common method as
	//
	// std::int64_t getTimeInMillis() const
	// std::int64_t getTimeInMicros() const
	// std::int64_t getTimeInNanos() const
	//
	// Such timers will be used later in the other class with such template methods assumed.
	//

	//
	// Define a high resolution timer.
	//
	class HighResTimer
	{
	public:

		//
		// Get the current time as no. of milliseconds from epoch.
		//
		// return : The current time as no. of milliseconds from epoch.
		//
		inline std::int64_t getTimeInMillis() const
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
		}

		//
		// Get the current time as no. of microseconds from epoch.
		//
		// return : The current time as no. of microseconds from epoch.
		//
		inline std::int64_t getTimeInMicros() const
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::microseconds(1);
		}

		//
		// Get the current time as no. of milliseconds from epoch.
		//
		// return : The current time as no. of milliseconds from epoch.
		//
		inline std::int64_t getTimeInNanos() const
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::nanoseconds(1);
		}
	};
}

#endif /* LIB_COMMON_SRC_TIMER_HPP_ */
