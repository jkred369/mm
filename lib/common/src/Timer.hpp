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
		inline std::uint64_t getTimeInMillis() const
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
		}

		//
		// Get the current time as no. of microseconds from epoch.
		//
		// return : The current time as no. of microseconds from epoch.
		//
		inline std::uint64_t getTimeInMicros() const
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::microseconds(1);
		}

		//
		// Get the current time as no. of milliseconds from epoch.
		//
		// return : The current time as no. of milliseconds from epoch.
		//
		inline std::uint64_t getTimeInNanos() const
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::nanoseconds(1);
		}
	};

	//
	// Define a high resolution timer.
	//
	class RdtscTimer
	{
	public:

		// How many cycles in a nanosecond
		static constexpr double CYCLE_TO_NANOS = 3.4;

		// How many cycles in a microsecond
		static constexpr std::int64_t CYCLE_TO_MICROS = 3400;

		// How many cycles in a millisecond
		static constexpr std::int64_t CYCLE_TO_MILLIS = 3400000;

		//
		// Get the cycles from rdtsc.
		//
		// The no. of cycles.
		//
		inline std::uint64_t getCycles() const
		{
			std::uint32_t eax, edx;
			asm volatile ("rdtsc" : "=a" (eax), "=d" (edx) : : "memory");
			return (((std::uint64_t) eax) | (((std::uint64_t) edx) << 32));
		}

		//
		// Get the current time as no. of milliseconds from epoch.
		//
		// return : The current time as no. of milliseconds from epoch.
		//
		inline std::uint64_t getTimeInMillis() const
		{
			return getCycles() / CYCLE_TO_MILLIS;
		}

		//
		// Get the current time as no. of microseconds from epoch.
		//
		// return : The current time as no. of microseconds from epoch.
		//
		inline std::uint64_t getTimeInMicros() const
		{
			return getCycles() / CYCLE_TO_MICROS;
		}

		//
		// Get the current time as no. of milliseconds from epoch.
		//
		// return : The current time as no. of milliseconds from epoch.
		//
		inline std::uint64_t getTimeInNanos() const
		{
			return getCycles() / CYCLE_TO_NANOS;
		}
	};

}

#endif /* LIB_COMMON_SRC_TIMER_HPP_ */
