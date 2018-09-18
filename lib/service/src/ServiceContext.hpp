/*
 * IServiceContext.hpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_SERVICECONTEXT_HPP_
#define LIB_SERVICE_SRC_SERVICECONTEXT_HPP_

#include <memory>
#include <unordered_map>
#include <string>

#include <DispatchKey.hpp>
#include <IConfig.hpp>
#include <IConsumer.hpp>
#include <ISubscription.hpp>
#include <Logger.hpp>

#include "IService.hpp"
#include "IServiceFactory.hpp"

namespace mm
{
	//
	// This class defines the service context interface.
	//
	class ServiceContext
	{
	public:

		//
		// Constructor. Initialize all the services available. Essentially factory method for a context.
		//
		// configFilePath : The configuration file path.
		//
		ServiceContext(const std::string configFilePath, IServiceFactory& factory);

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
			auto it = serviceMap.find(serviceName);
			if (it == serviceMap.end())
			{
				return false;
			}

			ServiceType* result = dynamic_cast<ServiceType*> (it->get());
			if (result != nullptr)
			{
				service = std::static_pointer_cast<ServiceType> (*it);
			}

			return result != nullptr;
		}

		//
		// Make the consumer subscribe to the given subscription.
		//
		// subscription : The subscription.
		// consumer : The consumer subscribing to it.
		//
		template<typename Message> bool subscribe(BaseSubscription subscription, const std::shared_ptr<IConsumer<Message> >& consumer)
		{
			for (std::pair<std::string, std::shared_ptr<IService> >& pair : serviceMap)
			{
				if (ISubscriber<Message>* subscriber = dynamic_cast<ISubscriber<Message>*> (pair.second.get()))
				{
					if (subscriber->subscribe(subscription, consumer))
					{
						return true;
					}
				}
			}

			return false;
		}

	private:

		// The logger for this class.
		static Logger logger;

		// The global config
		std::shared_ptr<IConfig> config;

		// The map where key is the service name and value is the service object.
		std::unordered_map<std::string, std::shared_ptr<IService> > serviceMap;
	};
}



#endif /* LIB_SERVICE_SRC_SERVICECONTEXT_HPP_ */
