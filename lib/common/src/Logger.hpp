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
#include "SpdLogger.hpp"

namespace mm
{
	typedef std::shared_ptr<spdlog::logger> Logger;
	typedef SpdLoggerSingleton LoggerSingleton;

#define LOG(level, ...) { if (!logger.get()) {logger = LoggerSingleton::getLogger(); } if (logger.get()) { logger->level(__VA_ARGS__); } }

#define LOGTRACE(...) LOG(trace, __VA_ARGS__)
#define LOGDEBUG(...) LOG(debug, __VA_ARGS__)
#define LOGINFO(...) LOG(info, __VA_ARGS__)
#define LOGWARN(...) LOG(warn, __VA_ARGS__)
#define LOGERR(...) LOG(error, __VA_ARGS__)
#define LOGFATAL(...) LOG(critical, __VA_ARGS__)

}

#endif /* LIB_COMMON_SRC_LOGGER_HPP_ */
