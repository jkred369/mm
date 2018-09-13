/*
 * Log.hpp
 *
 *  Created on: Sep 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_LOG_HPP_
#define LIB_COMMON_SRC_LOG_HPP_

#include <iostream>

namespace mm
{
	//
	// The log level enum.
	//
	enum class LogLevel : int
	{
		FATAL = 0,
		ERR = 1,
		WARN = 2,
		INFO = 3,
		DEBUG = 4,
		TRACE = 5,
	};

	//
	// Placeholder for quick build up.
	//
	// return : The current global log level.
	//
	inline LogLevel getLevel()
	{
		return LogLevel::DEBUG;
	}

#define LOG_IF(level) if (getLevel() >= level) { std::cout
#define ENDLOG std::endl; }

#define LOGTRACE LOG_IF(LogLevel::TRACE)
#define LOGDEBUG LOG_IF(LogLevel::DEBUG)
#define LOGINFO LOG_IF(LogLevel::INFO)
#define LOGWARN LOG_IF(LogLevel::WARN)
#define LOGERR LOG_IF(LogLevel::ERR)
#define LOGFATAL LOG_IF(LogLevel::FATAL)

}

#endif /* LIB_COMMON_SRC_LOG_HPP_ */
