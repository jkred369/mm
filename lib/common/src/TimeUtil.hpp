/*
 * TimeUtil.hpp
 *
 *  Created on: Oct 17, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_TIMEUTIL_HPP_
#define LIB_COMMON_SRC_TIMEUTIL_HPP_

#include <chrono>
#include <cstdint>

namespace mm
{
	template <typename T> struct is_chrono_duration
	{
		static constexpr bool value = false;
	};

	template <typename Rep, typename Period> struct is_chrono_duration<std::chrono::duration<Rep, Period> >
	{
		static constexpr bool value = true;
	};

	template<typename Duration> std::int64_t durationToEpochNanos(const Duration& duration)
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	}

	template<typename Duration> std::int64_t durationToEpochMillis(const Duration& duration)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	};

}

#endif /* LIB_COMMON_SRC_TIMEUTIL_HPP_ */
