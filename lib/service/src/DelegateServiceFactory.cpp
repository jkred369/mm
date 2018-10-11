/*
 * DelegateServiceFactory.cpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#include "DelegateServiceFactory.hpp"

mm::Logger mm::DelegateServiceFactory::logger;

namespace mm
{
	std::shared_ptr<IService> DelegateServiceFactory::createService(
			const std::string serviceClass,
			const std::string serviceName,
			const std::shared_ptr<IConfig> config,
			ServiceContext& context)
	{
		auto it = factoryMap.find(serviceClass);
		if (it != factoryMap.end())
		{
			return it->second->createService(serviceClass, serviceName, config, context);
		}

		LOGERR("Cannot find factory for service class: {}", serviceClass);
		return std::shared_ptr<IService>();
	}

	bool DelegateServiceFactory::registerFactory(const std::string& className, IServiceFactory* factory)
	{
		if (factory == nullptr)
		{
			LOGERR("Cannot add null factory for class: {}", className);
			return false;
		}

		auto it = factoryMap.find(className);
		if (it != factoryMap.end())
		{
			LOGWARN("Overriding factory in map for service class: {}", className);
		}

		factoryMap[className].reset(factory);
		return true;
	}

	DelegateServiceFactory& DelegateServiceFactory::getFactory()
	{
		static DelegateServiceFactory factory;
		return factory;
	}
}
