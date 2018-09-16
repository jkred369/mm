/*
 * SpgLogger.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#include <SpgLogger.hpp>

#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace mm
{
	SpgLogger::SpgLogger(const std::string& path, const std::string& loggerName)
	{
	    spdlog::init_thread_pool(8192, 1);

	    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, 1024 * 1024 * 10, 10);
	    std::vector<spdlog::sink_ptr> sinks {rotating_sink};

		logger = std::make_shared<spdlog::async_logger>(loggerName, sinks.begin(), sinks.end(),
				spdlog::thread_pool(), spdlog::async_overflow_policy::block);

		spdlog::register_logger(logger);
	}

	std::shared_ptr<spglog::logger> SpgLogger::getLogger()
	{
	}

}


