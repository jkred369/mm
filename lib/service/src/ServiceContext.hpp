/*
 * IServiceContext.hpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_SERVICECONTEXT_HPP_
#define LIB_SERVICE_SRC_SERVICECONTEXT_HPP_

#include <istream>
#include <memory>
#include <unordered_map>
#include <string>

#include <DispatchKey.hpp>
#include <Dispatcher.hpp>
#include <IConfig.hpp>
#include <IConsumer.hpp>
#include <IPublisher.hpp>
#include <Logger.hpp>
#include <Subscription.hpp>

#include "IService.hpp"
#include "IServiceFactory.hpp"

namespace mm
{
	//
	// This class defines the service context.
	//
	class ServiceContext : public IService
	{
	public:

		// The service list config name.
		static const std::string SERVICE_LIST;

		// The class of a service.
		static const std::string SERVICE_CLASS;

		//
		// Constructor. Initialize all the services available. Essentially factory method for a context.
		//
		// configFilePath : The configuration file path.
		// factory : The service factory instance.
		//
		ServiceContext(const std::string configFilePath, IServiceFactory& factory);

		//
		// Constructor. Initialize all the services available. Essentially factory method for a context.
		//
		// is : The input stream for the config file.
		// factory : The service factory instance.
		//
		ServiceContext(std::istream& is, IServiceFactory& factory);

		//
		// Constructor. Initialize all the services available. Essentially factory method for a context.
		//
		// is : The input stream for the config file.
		// factory : The service factory instance.
		//
		ServiceContext(std::istream&& is, IServiceFactory& factory);

		//
		// Constructor. Initialize all the services available. Essentially factory method for a context.
		//
		// config : The config instance.
		// factory : The service factory instance.
		//
		ServiceContext(const std::shared_ptr<IConfig>& config, IServiceFactory& factory);

		// forbide copy
		ServiceContext(const ServiceContext& rhs) = delete;

		//
		// IService implementation
		//
		virtual bool start();
		virtual void stop();

		//
		// Get the global dispatcher.
		//
		// return : the global dispatcher.
		//
		inline Dispatcher& getDispatcher() const
		{
			return *dispatcher;
		}

		//
		// Get the service with the given name.
		//
		// serviceName : name of the service.
		// service : The output pointer.
		//
		// return : true if the service can be found.
		//
		template<typename ServiceType> bool getService(const std::string& serviceName, ServiceType*& service)
		{
			auto it = serviceMap.find(serviceName);
			if (it == serviceMap.end())
			{
				return false;
			}

			ServiceType* result = dynamic_cast<ServiceType*> (it->second.get());
			if (result != nullptr)
			{
				service = result;
			}

			return result != nullptr;
		}

		//
		// Make the consumer subscribe to the given subscription.
		//
		// subscription : The subscription.
		// consumer : The consumer subscribing to it.
		//
		template<typename Message> bool subscribe(const Subscription& subscription, IConsumer<Message>* consumer)
		{
			if (consumer == nullptr)
			{
				LOGERR("Attempt to subscribe with null consumer to {}-{}-{}", toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
				return false;
			}

			for (std::pair<const std::string, std::shared_ptr<IService> >& pair : serviceMap)
			{
				if (IPublisher<Message>* subscriber = dynamic_cast<IPublisher<Message>*> (pair.second.get()))
				{
					if (subscriber->subscribe(subscription, consumer))
					{
						return true;
					}
				}
			}

			return false;
		}

		//
		// Make the consumer subscribe to the given subscription from all possible publishers.
		//
		// subscription : The subscription.
		// consumer : The consumer subscribing to it.
		//
		// return : The count of subscription made.
		//
		template<typename Message> std::int64_t subscribeAll(const Subscription& subscription, IConsumer<Message>* consumer)
		{
			if (consumer == nullptr)
			{
				LOGERR("Attempt to subscribe with null consumer to {}-{}-{}", toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
				return false;
			}

			int count = 0;

			for (std::pair<const std::string, std::shared_ptr<IService> >& pair : serviceMap)
			{
				if (IPublisher<Message>* subscriber = dynamic_cast<IPublisher<Message>*> (pair.second.get()))
				{
					if (subscriber->subscribe(subscription, consumer))
					{
						++count;
					}
				}
			}

			return count;
		}

	protected:

		//
		// Add a service to the context.
		//
		// serviceName : The service name.
		// service : The service.
		//
		// return : True if the service has been added.
		//
		inline bool setService(const std::string& serviceName, const std::shared_ptr<IService>& service)
		{
			if (serviceMap.find(serviceName) != serviceMap.end())
			{
				LOGERR("Service with name {} already exist. Skipped.", serviceName);
				return false;
			}

			serviceMap[serviceName] = service;
			LOGINFO("Service {} added to context.", serviceName);

			return true;
		}

	private:

		// The logger for this class.
		static Logger logger;

		// The global config
		std::shared_ptr<IConfig> config;

		// The global dispatcher.
		std::shared_ptr<Dispatcher> dispatcher;

		// The map where key is the service name and value is the service object.
		std::unordered_map<std::string, std::shared_ptr<IService> > serviceMap;
	};
}



#endif /* LIB_SERVICE_SRC_SERVICECONTEXT_HPP_ */
