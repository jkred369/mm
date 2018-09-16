/*
 * SpgLogger.hpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_SPGLOGGER_HPP_
#define LIB_COMMON_SRC_SPGLOGGER_HPP_

#include <memory>

#include "spdlog/sinks/stdout_color_sinks.h"

namespace mm
{
	//
	// The wrapper for spglog as a singleton per process.
	//
	class SpgLogger
	{
	public:

		//
		// Constructor.
		//
		// path : The log file path.
		// loggerName : The name of the logger.
		//
		SpgLogger(const std::string& path, const std::string& loggerName);

		//
		// Get the logger.
		//
		// return : The spg logger.
		//
		std::shared_ptr<spglog::logger> getLogger();

	private:

		// Pointer to the underlying logger.
		std::shared_ptr<spglog::logger> logger;
	};

	class SpgLoggerSingleton
	{
	public:

		//
		// Initialize the global logger singleton.
		//
		// path : The log file path.
		// loggerName : The logger name.
		//
		static void init(const std::string& path, const std::string& loggerName);

		//
		// Get the logger.
		//
		// return : The logger for logging.
		//
		static std::shared_ptr<spglog::logger> getLogger();

	private:

		// The file path.
		static std::string path;

		// The logger name.
		static std::string loggerName;
	};

}



#endif /* LIB_COMMON_SRC_SPGLOGGER_HPP_ */
