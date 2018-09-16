/*
 * IServiceContext.hpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_ISERVICECONTEXT_HPP_
#define LIB_SERVICE_SRC_ISERVICECONTEXT_HPP_

#include <IService.hpp>

namespace mm
{
	//
	// This class defines the service context interface.
	//
	//
	//
	class IServiceContext
	{
	public:

		virtual IServiceContext() {}

		//
		// Get the service with the given name.
		//
		// serviceName : name of the service.
		// service : The output pointer.
		//
		// return : true if the service can be found.
		//
		template<typename ServiceType> bool getService(const std::string& serviceName, std::shared_ptr<ServiceType>& service)
		{
			ServiceType* result = dynamic_cast<ServiceType*> (getService(serviceName));
			if (result != nullptr)
			{
				service = result;
			}

			return result != nullptr;
		}

	protected:

		//
		// Get the service with the given name.
		//
		// serviceName : The service name.
		//
		// return : The service with the given name. nullptr if the service cannot be found.
		//
		virtual IService* getService(const std::string& serviceName) = 0;

	};
}



#endif /* LIB_SERVICE_SRC_ISERVICECONTEXT_HPP_ */
