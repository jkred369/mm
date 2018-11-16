/*
 * Logger.hpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_LOGGER_HPP_
#define LIB_COMMON_SRC_LOGGER_HPP_

#include <memory>

#include "LogLevel.hpp"
#include "NativeDefinition.hpp"
#include "SpdLogger.hpp"
#include "TimeUtil.hpp"
#include "StringUtil.hpp"

namespace mm
{
	typedef std::shared_ptr<spdlog::logger> Logger;
	typedef SpdLoggerSingleton LoggerSingleton;

#define LOG_STR_H(x) #x
#define LOG_STR_HELPER(x) LOG_STR_H(x)

// Turn off the file and line based logging by comment out the following
#define LOG_INCLUDE_LOCATION

#ifdef LOG_INCLUDE_LOCATION
#define LOG(level, fmt, ...) { if (UNLIKELY(!logger.get())) { logger = LoggerSingleton::getLogger(); } if (LIKELY(logger.get())) { logger->level(fmt " ({}:{})", ## __VA_ARGS__, StringUtil::fileNameFromPath(__FILE__), __LINE__); } }
#else
#define LOG(level, ...) { if (UNLIKELY(!logger.get())) {logger = LoggerSingleton::getLogger(); } if (LIKELY(logger.get())) { logger->level(__VA_ARGS__); } }
#endif

#define LOGTRACE(...) LOG(trace, __VA_ARGS__)
#define LOGDEBUG(...) LOG(debug, __VA_ARGS__)
#define LOGINFO(...) LOG(info, __VA_ARGS__)
#define LOGWARN(...) LOG(warn, __VA_ARGS__)
#define LOGERR(...) LOG(error, __VA_ARGS__)
#define LOGFATAL(...) LOG(critical, __VA_ARGS__)

	//
	// Insert the time stamp so the log name becomes (assuming its in *.log format)
	//
	// *.yyyyMMddHHmmss.log
	//
	// logName : The original log name.
	//
	// return : The log name with timestamp.
	//
	inline const std::string getTimedLogName(const std::string& logName)
	{
		const std::time_t curTime = std::time(0);
		const std::string timestamp = getTimeString(curTime, "%Y%m%d-%H%M%S");

		const std::string::size_type index = logName.find('.');
		if (index == std::string::npos)
		{
			return logName + "." + timestamp;
		}
		else if (index == logName.size() - 1)
		{
			return logName + timestamp;
		}

		return logName.substr(0, index) + "." + timestamp + logName.substr(index);
	}

}

#endif /* LIB_COMMON_SRC_LOGGER_HPP_ */
