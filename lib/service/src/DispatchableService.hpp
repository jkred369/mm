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
#include <IService.hpp>
#include <ServiceContext.hpp>

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
		//
		DispatchableService(
				const KeyType dispatchKey,
				const std::shared_ptr<Dispatcher> dispatcher,
				const std::shared_ptr<ServiceContext> serviceContext);

		//
		// service interface.
		//
		virtual bool start();
		virtual void stop();

		//
		// dispatchable interface.
		//
		virtual const KeyType getKey() const;

	protected:

		// The dispatch key for the service.
		const KeyType dispatchKey;

		// The dispatcher used.
		const std::shared_ptr<Dispatcher> dispatcher;

		// The service context.
		const std::shared_ptr<ServiceContext> serviceContext;
	};
}



#endif /* LIB_SERVICE_SRC_DISPATCHABLESERVICE_HPP_ */
