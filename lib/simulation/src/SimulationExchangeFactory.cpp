/*
 * SimulationExchangeFactory.cpp
 *
 *  Created on: 12 Nov 2018
 *      Author: alexsuo
 */

#include <fstream>

#include <DispatchKey.hpp>
#include <DelegateServiceFactory.hpp>

#include "SimulationExchange.hpp"
#include "SimulationExchangeFactory.hpp"

const std::string mm::SimulationExchangeConfig::CLASS_NAME = "SimulationExchange";
const std::string mm::SimulationExchangeConfig::DISPATCH_KEY = "DispatchKey";
const std::string mm::SimulationExchangeConfig::PRODUCT_SERVICE_NAME = "ProductServiceName";
const std::string mm::SimulationExchangeConfig::MARKET_DATA_FILE = "MarketDataFile";

const bool mm::SimulationExchangeFactory::registered = mm::DelegateServiceFactory::getFactory().registerFactory(
		SimulationExchangeConfig::CLASS_NAME, new mm::SimulationExchangeFactory());

mm::Logger mm::SimulationExchangeFactory::logger;

namespace mm
{
	SimulationExchangeFactory::~SimulationExchangeFactory()
	{
	}

	std::shared_ptr<IService> SimulationExchangeFactory::createService(
			const std::string serviceClass,
			const std::string serviceName,
			const std::shared_ptr<IConfig> config,
			ServiceContext& context)
	{
		// sanity check
		if (serviceClass != SimulationExchangeConfig::CLASS_NAME)
		{
			LOGERR("Cannot create service with class: {}. Only support class: {}", serviceClass, SimulationExchangeConfig::CLASS_NAME);
			return std::shared_ptr<IService> ();
		}

		// creation
		const KeyType dispatchKey = config->getInt64(SimulationExchangeConfig::DISPATCH_KEY);
		const std::string productServiceName = config->getString(SimulationExchangeConfig::PRODUCT_SERVICE_NAME);
		const std::string marketDataFile = config->getString(SimulationExchangeConfig::MARKET_DATA_FILE);

		std::ifstream ifs(marketDataFile);
		return std::shared_ptr<IService> (new SimulationExchange(dispatchKey, serviceName, context, productServiceName, ifs));
	}
}


