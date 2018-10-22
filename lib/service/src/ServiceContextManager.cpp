/*
 * ServiceContextManager.cpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#include <iostream>
#include <signal.h>

#include "DelegateServiceFactory.hpp"
#include "ServiceContextManager.hpp"

mm::Logger mm::ServiceContextManager::logger;

namespace mm
{
	void ServiceContextManager::createContextAndStart(const std::string path)
	{
		bool shutDownFlag = false;

		// block the default signals
		{
			sigset_t signals;
			sigemptyset(&signals);
			sigaddset(&signals, SIGTERM);
			sigaddset(&signals, SIGUSR1);
			sigaddset(&signals, SIGUSR2);

			if (pthread_sigmask(SIG_BLOCK, &signals, NULL) != 0)
			{
				std::cerr << "Error blocking signal masks" << std::endl;
				shutDownFlag = true;
			}
		}

		// start the context
		DelegateServiceFactory& factory = DelegateServiceFactory::getFactory();
		context.reset(new ServiceContext(path, factory));

		if (!context->start())
		{
			LOGFATAL("Error starting context. Stopping...");
			shutDownFlag = true;
		}

		// block and shut down
		while (!shutDownFlag)
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

			if (result == SIGTERM || result == SIGUSR1 || result == SIGUSR2)
			{
				LOGERR("Received signal {}, shutting down ...", result);
				shutDownFlag = true;
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
