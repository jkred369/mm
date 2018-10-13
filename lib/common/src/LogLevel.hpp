/*
 * Log.hpp
 *
 *  Created on: Sep 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_LOGLEVEL_HPP_
#define LIB_COMMON_SRC_LOGLEVEL_HPP_

#include <iostream>
#include <string>

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

	// The log level strings.
	struct LogLevelConstant
	{
		static const std::string LOG_TRACE;
		static const std::string LOG_DEBUG;
		static const std::string LOG_INFO;
		static const std::string LOG_WARN;
		static const std::string LOG_ERR;
		static const std::string LOG_FATAL;
		static const std::string LOG_UNKNOWN;

		//
		// Get the log level name from the value.
		//
		// return : string presentation of the log level.
		//
		static const std::string& getName(LogLevel level);

		//
		// Get the log level value from name.
		//
		// return : log level.
		//
		static LogLevel getLevel(const std::string& name);
	};

	//
	// The configuration constants for loggers.
	//
	struct LoggerConfig
	{
		// The log config group name
		static const std::string LOG_CONFIG;

		// The log path
		static const std::string LOG_PATH;

		// The default log path
		static const std::string DEFAULT_LOG_PATH;

		// The log level
		static const std::string LOG_LEVEL;

		// The logger name
		static const std::string LOG_NAME;

		// The default logger name.
		static const std::string DEFAULT_LOG_NAME;

		// The log pattern
		static const std::string LOG_PATTERN;
	};

}

#endif /* LIB_COMMON_SRC_LOGLEVEL_HPP_ */
