/*
 * SimulationExchangeFactory.hpp
 *
 *  Created on: 12 Nov 2018
 *      Author: alexsuo
 */

#ifndef LIB_SIMULATION_SRC_SIMULATIONEXCHANGEFACTORY_HPP_
#define LIB_SIMULATION_SRC_SIMULATIONEXCHANGEFACTORY_HPP_

#include <string>

#include <IServiceFactory.hpp>
#include <Logger.hpp>

namespace mm
{
	struct SimulationExchangeConfig
	{
		// The class name for sanity check.
		static const std::string CLASS_NAME;

		// The dispatch key for simulation exchange.
		static const std::string DISPATCH_KEY;

		// The product service name.
		static const std::string PRODUCT_SERVICE_NAME;

		// The data file path for the simulation exchange.
		static const std::string MARKET_DATA_FILE;

	};

	//
	// The factory for simulation exchange.
	//
	class SimulationExchangeFactory : public IServiceFactory
	{
	public:

		// Flag if the factory is registered.
		static const bool registered;

		// virtual destructor.
		virtual ~SimulationExchangeFactory();

		//
		// Implement the service factory.
		//
		virtual std::shared_ptr<IService> createService(
				const std::string serviceClass,
				const std::string serviceName,
				const std::shared_ptr<IConfig> config,
				ServiceContext& context) override;

	private:

		// Logger of the class.
		static Logger logger;
	};
}



#endif /* LIB_SIMULATION_SRC_SIMULATIONEXCHANGEFACTORY_HPP_ */
