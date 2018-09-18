/*
 * IServiceFactory.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_ISERVICEFACTORY_HPP_
#define LIB_SERVICE_SRC_ISERVICEFACTORY_HPP_

#include <memory>

#include <IConfig.hpp>

#include "IService.hpp"

namespace mm
{
	//
	// Define service factory interface.
	//
	struct IServiceFactory
	{
		virtual ~IServiceFactory() {}

		//
		// Create service with given name.
		//
		// serviceName : The service name.
		// config : The config for the service.
		//
		// return : The service.
		//
		virtual IService* createService(const std::string serviceName, const std::shared_ptr<IConfig> config) = 0;
	};
}



#endif /* LIB_SERVICE_SRC_ISERVICEFACTORY_HPP_ */
