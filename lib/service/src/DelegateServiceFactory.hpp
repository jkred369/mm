/*
 * DelegateServiceFactory.hpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_DELEGATESERVICEFACTORY_HPP_
#define LIB_SERVICE_SRC_DELEGATESERVICEFACTORY_HPP_

#include <unordered_map>

#include <Logger.hpp>

#include "IServiceFactory.hpp"

namespace mm
{
	//
	// This class works as a aggregated service factory instance.
	//
	class DelegateServiceFactory : public IServiceFactory
	{
	public:

		//
		// factory interface.
		//
		virtual std::shared_ptr<IService> createService(const std::string serviceClass, const std::shared_ptr<IConfig> config, ServiceContext& context) override;

		//
		// register a factory.
		//
		void registerFactory(const std::string& className, IServiceFactory* factory);

		//
		// Get the global static factory.
		//
		static DelegateServiceFactory& getFactory();

	private:

		// Logger of the class.
		static Logger logger;

		// The delegators.
		std::unordered_map<std::string, IServiceFactory*> factoryMap;
	};
}



#endif /* LIB_SERVICE_SRC_DELEGATESERVICEFACTORY_HPP_ */
