/*
 * ServiceContext.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#include <fstream>
#include <stdexcept>

#include <PropertyConfig.hpp>

#include "ServiceContext.hpp"

const std::string mm::ServiceContext::SERVICE_LIST = "ServiceList";

namespace mm
{
	ServiceContext::ServiceContext(const std::string configFilePath, IServiceFactory& factory)
	{
		// initialize the config
		try
		{
			std::ifstream ifs(configFilePath);
			config.reset(new PropertyConfig(ifs));
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("Error loading configuration file: " + std::string(e.what()));
		}

		// logger
		{
			std::shared_ptr<IConfig> loggerConfig = config->getSubConfig(LoggerConfig::LOG_CONFIG);

			if (!loggerConfig.get())
			{
				throw std::runtime_error("No logger in config file: " + configFilePath);
			}

			const LogLevel level = LogLevelConstant::getLevel(loggerConfig->getString(LoggerConfig::LOG_LEVEL, LogLevelConstant::LOG_TRACE));
			const std::string& path = loggerConfig->getString(LoggerConfig::LOG_PATH);
			const std::string& name = loggerConfig->getString(LoggerConfig::LOG_NAME);

			LoggerSingleton::init(level, path, name);

			// first chance to log here after logger creation
			LOGINFO("Config loaded from {}", configFilePath);
			LOGINFO("Logger {} created with level {}, log file {}", name, LogLevelConstant::getName(level), path);
		}

		// dispatcher threading model
		{
			std::shared_ptr<IConfig> dispatcherConfig = config->getSubConfig(DispatcherConfig::DISPATCHER_CONFIG);

			if (!dispatcherConfig.get())
			{
				throw std::runtime_error("No dispatcher section in config file: " + configFilePath);
			}

			const int threadCount = dispatcherConfig->getInt64(DispatcherConfig::THREAD_COUNT);
			if (threadCount <= 0)
			{
				LOGERR("Invalid thread count for dispatcher: {}", threadCount);
			}

			dispatcher.reset(new Dispatcher(threadCount, false));
			LOGINFO("Dispatcher created with {} threads.", threadCount);
		}

		// create services
		{
			const std::vector<std::string> serviceList = config->getStringList(ServiceContext::SERVICE_LIST);
			if (serviceList.empty())
			{
				throw std::runtime_error("No service specified in " + ServiceContext::SERVICE_LIST);
			}

			for (const std::string& serviceName : serviceList)
			{
				std::shared_ptr<IConfig> serviceConfig = config->getSubConfig(serviceName);

				if (!serviceConfig.get())
				{
					throw std::runtime_error("No config for service " + serviceName);
				}

				serviceMap[serviceName] = factory.createService(serviceName, serviceConfig, *this);
				LOGINFO("Service {} created successfully.", serviceName);
			}
		}

		LOGINFO("Service context created.");
	}

	bool ServiceContext::start()
	{
		dispatcher->start();
		LOGINFO("Dispatcher started.");

		for (std::pair<const std::string, std::shared_ptr<IService> >& pair : serviceMap)
		{
			if (!pair.second->start())
			{
				LOGERR("Error starting service {}", pair.first);
				return false;
			}

			LOGINFO("Service {} started.", pair.first);
		}

		LOGINFO("Service context started.");
		return true;
	}

	void ServiceContext::stop()
	{
		dispatcher->stop();
		LOGINFO("Dispatcher stopped.");

		for (std::pair<const std::string, std::shared_ptr<IService> >& pair : serviceMap)
		{
			pair.second->stop();
			LOGINFO("Service {} stopped.", pair.first);
		}

		LOGINFO("Service context stopped.");
	}

}


