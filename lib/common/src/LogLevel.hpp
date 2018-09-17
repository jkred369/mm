/*
 * Log.hpp
 *
 *  Created on: Sep 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_LOGLEVEL_HPP_
#define LIB_COMMON_SRC_LOGLEVEL_HPP_

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
	};

}

#endif /* LIB_COMMON_SRC_LOGLEVEL_HPP_ */
