/*
 * ServiceContextManager.cpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#include <csignal>
#include <signal.h>

#include "DelegateServiceFactory.hpp"
#include "ServiceContextManager.hpp"

mm::Logger mm::ServiceContextManager::logger;
std::atomic<bool> mm::ServiceContextManager::shutDownFlag;

namespace mm
{
	void ServiceContextManager::handle(int signal)
	{
		if (signal == SIGTERM || signal == SIGUSR1)
		{
			shutDownFlag.store(true);
		}
	}

	void ServiceContextManager::createContextAndStart(const std::string path)
	{
		shutDownFlag.store(false);

		// register the signal handler
		std::signal(SIGTERM, handle);
		std::signal(SIGUSR1, handle);

		// start the context
		DelegateServiceFactory& factory = DelegateServiceFactory::getFactory();
		context.reset(new ServiceContext(path, factory));

		if (!context->start())
		{
			LOGFATAL("Error starting context. Stopping...");
			shutDownFlag.store(true);
		}

		// block and shut down
		while (!shutDownFlag.load())
		{
			sigset_t signals;
			sigemptyset(&signals);
			sigaddset(&signals, SIGTERM);
			sigaddset(&signals, SIGUSR1);
			sigaddset(&signals, SIGUSR2);

			int result = 0;
			if (sigwait(&signals, &result) != 0)
			{
				LOGERR("Error on signal waiting: {}", result);
			}
		}

		shutdown();
	}

	void ServiceContextManager::shutdown()
	{
		// call to make sure the important logs are written down.
		logger->flush();

		LOGINFO("Shutting down service context");
		context->stop();
		LOGINFO("Service context shut down successfully");

		// final attempt
		logger->flush();
	}

}
