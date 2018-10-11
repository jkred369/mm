/*
 * ProductServiceFactory.cpp
 *
 *  Created on: Oct 11, 2018
 *      Author: suoalex
 */

#include <fstream>

#include <DispatchKey.hpp>
#include <DelegateServiceFactory.hpp>

#include "ProductService.hpp"
#include "ProductServiceFactory.hpp"

const std::string mm::ProductServiceConfig::CLASS_NAME = "ProductService";
const std::string mm::ProductServiceConfig::DISPATCH_KEY = "DispatchKey";
const std::string mm::ProductServiceConfig::DATA_FILE = "DataFile";

const bool mm::ProductServiceFactory::registered = mm::DelegateServiceFactory::getFactory().registerFactory(
		ProductServiceConfig::CLASS_NAME, new mm::ProductServiceFactory());

mm::Logger mm::ProductServiceFactory::logger;

namespace mm
{
	ProductServiceFactory::~ProductServiceFactory()
	{
	}

	std::shared_ptr<IService> ProductServiceFactory::createService(
			const std::string serviceClass,
			const std::string serviceName,
			const std::shared_ptr<IConfig> config,
			ServiceContext& context)
	{
		// sanity check
		if (serviceClass != ProductServiceConfig::CLASS_NAME)
		{
			LOGERR("Cannot create service with class: {}. Only support class: {}", serviceClass, ProductServiceConfig::CLASS_NAME);
			return std::shared_ptr<IService> ();
		}

		// creation
		const KeyType dispatchKey = config->getInt64(ProductServiceConfig::DISPATCH_KEY);
		const std::string dataFile = config->getString(ProductServiceConfig::DATA_FILE);

		std::ifstream ifs(dataFile);
		return std::shared_ptr<IService> (new ProductService(dispatchKey, serviceName, context, ifs));
	}
}


