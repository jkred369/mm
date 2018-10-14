/*
 * FemasOrderSessionFactory.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: suoalex
 */

#include <DispatchKey.hpp>
#include <DelegateServiceFactory.hpp>

#include "FemasOrderSession.hpp"
#include "FemasOrderSessionFactory.hpp"

const std::string mm::FemasOrderSessionConfig::CLASS_NAME = "FemasOrderSession";
const std::string mm::FemasOrderSessionConfig::DISPATCH_KEY = "DispatchKey";
const std::string mm::FemasOrderSessionConfig::PRODUCT_SERVICE_NAME = "ProductServiceName";

const bool mm::FemasOrderSessionFactory::registered = mm::DelegateServiceFactory::getFactory().registerFactory(
		mm::FemasOrderSessionConfig::CLASS_NAME, new mm::FemasOrderSessionFactory());

mm::Logger mm::FemasOrderSessionFactory::logger;

namespace mm
{
	FemasOrderSessionFactory::~FemasOrderSessionFactory()
	{
	}

	std::shared_ptr<IService> FemasOrderSessionFactory::createService(
			const std::string serviceClass,
			const std::string serviceName,
			const std::shared_ptr<IConfig> config,
			ServiceContext& context)
	{
		// sanity check
		if (serviceClass != FemasOrderSessionConfig::CLASS_NAME)
		{
			LOGERR("Cannot create service with class: {}. Only support class: {}", serviceClass, FemasOrderSessionConfig::CLASS_NAME);
			return std::shared_ptr<IService> ();
		}

		// creation
		const KeyType dispatchKey = config->getInt64(FemasOrderSessionConfig::DISPATCH_KEY);
		const std::string productServiceName = config->getString(FemasOrderSessionConfig::PRODUCT_SERVICE_NAME);
		const FemasUserDetail userDetail(config);
		const FemasOrderDetail orderDetail(config);

		return std::shared_ptr<IService> (new FemasOrderSession(dispatchKey, serviceName, context, productServiceName, userDetail, orderDetail));
	}
}


