/*
 * IServiceFactory.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_ISERVICEFACTORY_HPP_
#define LIB_SERVICE_SRC_ISERVICEFACTORY_HPP_

#include <memory>
#include <string>

#include <IConfig.hpp>

#include "IService.hpp"

namespace mm
{
	class ServiceContext;

	//
	// Define service factory interface.
	//
	struct IServiceFactory
	{
		virtual ~IServiceFactory() {}

		//
		// Create service with given name.
		//
		// serviceClass : The service class.
		// serviceName : The service name.
		// config : The config for the service.
		// context : The service context for sourcing other services.
		//
		// return : The service as a shared pointer.
		//
		virtual std::shared_ptr<IService> createService(
				const std::string serviceClass,
				const std::string serviceName,
				const std::shared_ptr<IConfig> config,
				ServiceContext& context) = 0;
	};
}



#endif /* LIB_SERVICE_SRC_ISERVICEFACTORY_HPP_ */
