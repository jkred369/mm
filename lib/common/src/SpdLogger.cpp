/*
 * SpgLogger.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "SpdLogger.hpp"

mm::LogLevel mm::SpgLoggerSingleton::level = mm::LogLevel::TRACE;
std::string mm::SpgLoggerSingleton::path = "";
std::string mm::SpgLoggerSingleton::loggerName = "";

namespace mm
{
	SpdLogger::SpdLogger(LogLevel level, const std::string& path, const std::string& loggerName)
	{
	    spdlog::init_thread_pool(8192, 1);

	    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, 1024 * 1024 * 10, 10);
	    std::vector<spdlog::sink_ptr> sinks {rotating_sink};

		logger = std::make_shared<spdlog::async_logger>(loggerName, sinks.begin(), sinks.end(),
				spdlog::thread_pool(), spdlog::async_overflow_policy::block);

		spdlog::register_logger(logger);
		setLevel(level);
	}

	void SpdLogger::setLevel(LogLevel level)
	{
		spdlog::level spdLevel;

		switch (level)
		{
		case LogLevel::TRACE:
			spdLevel = spdlog::level::trace;
			break;
		case LogLevel::DEBUG:
			spdLevel = spdlog::level::debug;
			break;
		case LogLevel::INFO:
			spdLevel = spdlog::level::info;
			break;
		case LogLevel::WARN:
			spdLevel = spdlog::level::warn;
			break;
		case LogLevel::ERR:
			spdLevel = spdlog::level::err;
			break;
		case LogLevel::FATAL:
			spdLevel = spdlog::level::critical;
			break;
		default:
			spdLevel = spdlog::level::trace;
		}

		logger->set_level(spdLevel);
		logger->info("Logging level set to {}", mm::LogLevelConstant::getName(level));
	}

	void SpdLoggerSingleton::init(LogLevel level, const std::string& path, const std::string& loggerName)
	{
		SpdLoggerSingleton::level = level;
		SpdLoggerSingleton::path = path;
		SpdLoggerSingleton::loggerName = loggerName;
	}

	std::shared_ptr<spdlog::logger> SpdLoggerSingleton::getLogger()
	{
		static SpdLogger logger(level, path, loggerName);
		return logger->getLogger();
	}

}


