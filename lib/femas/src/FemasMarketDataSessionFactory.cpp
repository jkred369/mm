/*
 * FemasMarketDataSessionFactory.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: suoalex
 */

#include <DispatchKey.hpp>
#include <DelegateServiceFactory.hpp>
#include <ThreadUtil.hpp>

#include "FemasMarketDataSession.hpp"
#include "FemasMarketDataSessionFactory.hpp"

const std::string mm::FemasMarketDataSessionConfig::CLASS_NAME = "FemasMarketDataSession";
const std::string mm::FemasMarketDataSessionConfig::DISPATCH_KEY = "DispatchKey";
const std::string mm::FemasMarketDataSessionConfig::PRODUCT_SERVICE_NAME = "ProductServiceName";
const std::string mm::FemasMarketDataSessionConfig::CPU_AFFINITY = "CpuAffinity";

const bool mm::FemasMarketDataSessionFactory::registered = mm::DelegateServiceFactory::getFactory().registerFactory(
		mm::FemasMarketDataSessionConfig::CLASS_NAME, new mm::FemasMarketDataSessionFactory());

mm::Logger mm::FemasMarketDataSessionFactory::logger;

namespace mm
{
	FemasMarketDataSessionFactory::~FemasMarketDataSessionFactory()
	{
	}

	std::shared_ptr<IService> FemasMarketDataSessionFactory::createService(
			const std::string serviceClass,
			const std::string serviceName,
			const std::shared_ptr<IConfig> config,
			ServiceContext& context)
	{
		// sanity check
		if (serviceClass != FemasMarketDataSessionConfig::CLASS_NAME)
		{
			LOGERR("Cannot create service with class: {}. Only support class: {}", serviceClass, FemasMarketDataSessionConfig::CLASS_NAME);
			return std::shared_ptr<IService> ();
		}

		// creation
		const KeyType dispatchKey = config->getInt64(FemasMarketDataSessionConfig::DISPATCH_KEY);
		const std::string productServiceName = config->getString(FemasMarketDataSessionConfig::PRODUCT_SERVICE_NAME);
		const int cpuAffinity = config->getInt64(FemasMarketDataSessionConfig::CPU_AFFINITY, ThreadUtil::CPU_ID_NOT_SET);
		const FemasUserDetail userDetail(config);

		return std::shared_ptr<IService> (new FemasMarketDataSession(dispatchKey, serviceName, context, productServiceName, userDetail, cpuAffinity));
	}
}


