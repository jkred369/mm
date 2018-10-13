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
#include <cstring>
#include <ctime>
#include <string>

namespace mm
{
	template <typename T> struct is_chrono_time_point
	{
		static constexpr bool value = false;
	};

	template <typename Clock, typename Duration> struct is_chrono_time_point<std::chrono::time_point<Clock, Duration> >
	{
		static constexpr bool value = true;
	};

	template <typename T> struct is_chrono_duration
	{
		static constexpr bool value = false;
	};

	template <typename Rep, typename Period> struct is_chrono_duration<std::chrono::duration<Rep, Period> >
	{
		static constexpr bool value = true;
	};

	template<typename TimePoint> inline std::int64_t timePointToEpochNanos(const TimePoint& timePoint)
	{
	    static_assert(is_chrono_time_point<TimePoint>::value, "TimePoint must be a std::chrono::time_point");
	    return timePoint.time_since_epoch() / std::chrono::nanoseconds(1);
	};

	template<typename TimePoint> inline std::int64_t timePointToEpochMillis(const TimePoint& timePoint)
	{
	    static_assert(is_chrono_time_point<TimePoint>::value, "TimePoint must be a std::chrono::time_point");
	    return timePoint.time_since_epoch() / std::chrono::milliseconds(1);
	};

	template<typename Duration> inline std::int64_t durationToNanos(const Duration& duration)
	{
	    static_assert(is_chrono_duration<Duration>::value, "duration must be a std::chrono::duration");
		return std::chrono::duration_cast<std::chrono::nanoseconds>(duration) / std::chrono::nanoseconds(1);
	};

	template<typename Duration> inline std::int64_t durationToMillis(const Duration& duration)
	{
		static_assert(is_chrono_duration<Duration>::value, "duration must be a std::chrono::duration");
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration) / std::chrono::milliseconds(1);
	};

	const std::string inline getTimeString(const std::time_t time, const std::string& format)
	{
		char buffer[128];
		std::memset(buffer, 0, sizeof(buffer));

		std::strftime(buffer, sizeof(buffer), format.c_str(), std::localtime(&time));
		return std::string(buffer);
	}

}

#endif /* LIB_COMMON_SRC_TIMEUTIL_HPP_ */
