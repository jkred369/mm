/*
 * ServiceContextManager.hpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_SERVICECONTEXTMANAGER_HPP_
#define LIB_SERVICE_SRC_SERVICECONTEXTMANAGER_HPP_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include <Logger.hpp>

#include "ServiceContext.hpp"

namespace mm
{
	//
	// This class creates and manages the life time for a service context.
	//
	class ServiceContextManager
	{
	public:

		//
		// The signal handler for shut down.
		//
		static void handle(int signal);

		//
		// Create context and start the program. The calling thread would be blocked till context stop.
		//
		// path : The path for the configuration file.
		//
		void createContextAndStart(const std::string path);

		//
		// Shut down the context and release all the threads waiting for it to finish.
		//
		void shutdown();

	private:

		//
		// structure for shut down handling
		//
		struct ShutDownMonitor
		{
			// the mutex for locking
			std::mutex mutex;

			// the condition for raising
			std::condition_variable condition;
		};

		// Logger of the class.
		static Logger logger;

		// The shut down monitor
		static std::atomic<bool> shutDownFlag;

		// The context.
		std::unique_ptr<ServiceContext> context;

		// The mux for the condition.
		std::mutex mutex;

		// The condition to block.
		std::condition_variable condition;
	};
}



#endif /* LIB_SERVICE_SRC_SERVICECONTEXTMANAGER_HPP_ */
