/*
 * LogLevel.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#include <LogLevel.hpp>

const std::string mm::LOG_TRACE = "TRACE";
const std::string mm::LOG_DEBUG = "DEBUG";
const std::string mm::LOG_INFO = "INFO";
const std::string mm::LOG_WARN = "WARN";
const std::string mm::LOG_ERR = "ERR";
const std::string mm::LOG_FATAL = "FATAL";
const std::string mm::LOG_UNKNOWN = "UNKNOWN";

namespace mm
{
	const std::string& getName(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::TRACE:
			return LOG_TRACE;
		case LogLevel::DEBUG:
			return LOG_DEBUG;
		case LogLevel::INFO:
			return LOG_INFO;
		case LogLevel::WARN:
			return LOG_WARN;
		case LogLevel::ERR:
			return LOG_ERR;
		case LogLevel::FATAL:
			return LOG_FATAL;
		default:
			return LOG_UNKOWN;
		}
	}

}
