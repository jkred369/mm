/*
 * LogLevel.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#include "LogLevel.hpp"

const std::string mm::LogLevelConstant::LOG_TRACE = "TRACE";
const std::string mm::LogLevelConstant::LOG_DEBUG = "DEBUG";
const std::string mm::LogLevelConstant::LOG_INFO = "INFO";
const std::string mm::LogLevelConstant::LOG_WARN = "WARN";
const std::string mm::LogLevelConstant::LOG_ERR = "ERR";
const std::string mm::LogLevelConstant::LOG_FATAL = "FATAL";
const std::string mm::LogLevelConstant::LOG_UNKNOWN = "UNKNOWN";

const std::string mm::LoggerConfig::LOG_CONFIG = "Logger";
const std::string mm::LoggerConfig::LOG_PATH = "Path";
const std::string mm::LoggerConfig::LOG_LEVEL = "Level";
const std::string mm::LoggerConfig::LOG_NAME = "Name";

namespace mm
{
	const std::string& LogLevelConstant::getName(LogLevel level)
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
			return LOG_UNKNOWN;
		}
	}

	LogLevel LogLevelConstant::getLevel(const std::string& name)
	{
		if (name == LOG_TRACE)
		{
			return LogLevel::TRACE;
		}
		else if (name == LOG_DEBUG)
		{
			return LogLevel::DEBUG;
		}
		else if (name == LOG_INFO)
		{
			return LogLevel::INFO;
		}
		else if (name == LOG_WARN)
		{
			return LogLevel::WARN;
		}
		else if (name == LOG_ERR)
		{
			return LogLevel::ERR;
		}
		else if (name == LOG_FATAL)
		{
			return LogLevel::FATAL;
		}
		else
		{
			// by default use the lowest log level
			return LogLevel::TRACE;
		}
	}

}
