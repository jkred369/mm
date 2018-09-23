/*
 * DispatchableServce.hpp
 *
 *  Created on: Sep 19, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_DISPATCHABLESERVICE_HPP_
#define LIB_SERVICE_SRC_DISPATCHABLESERVICE_HPP_

#include <Dispatcher.hpp>
#include <IDispatchable.hpp>

#include "IService.hpp"
#include "ServiceContext.hpp"

namespace mm
{
	//
	// The class defines a dispatchable service.
	//
	class DispatchableService :
			public IService,
			public virtual IDispatchable
	{
	public:

		//
		// Constructor.
		//
		// dispatchKey : The dispatch key.
		// dispatcher : The dispatcher used.
		// serviceContext : The service context for subscription and source other service.
		// serviceName : The name of the service in context.
		//
		DispatchableService(
				const KeyType dispatchKey,
				const std::string contextName,
				const std::shared_ptr<Dispatcher> dispatcher,
				const std::shared_ptr<ServiceContext> serviceContext);

		//
		// service interface.
		//
		virtual bool start() override;
		virtual void stop() override;

		//
		// dispatchable interface.
		//
		virtual const KeyType getKey() const override;

		//
		// Get the service name.
		//
		// return : The service name.
		//
		inline const std::string& getName() const
		{
			return serviceName;
		}

	protected:

		// The dispatch key for the service.
		const KeyType dispatchKey;

		// The dispatcher used.
		const std::shared_ptr<Dispatcher> dispatcher;

		// The service context.
		const std::shared_ptr<ServiceContext> serviceContext;

		// The name of the service in context.
		const std::string serviceName;
	};
}



#endif /* LIB_SERVICE_SRC_DISPATCHABLESERVICE_HPP_ */
