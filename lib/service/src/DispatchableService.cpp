/*
 * DispatchableService.cpp
 *
 *  Created on: Sep 19, 2018
 *      Author: suoalex
 */

#include "DispatchableService.hpp"

namespace mm
{
	DispatchableService::DispatchableService(
			const KeyType dispatchKey,
			const std::string contextName,
			const std::shared_ptr<Dispatcher> dispatcher,
			const std::shared_ptr<ServiceContext> serviceContext) :
		dispatchKey(dispatchKey),
		dispatcher(dispatcher),
		serviceContext(serviceContext)
	{
	}

	bool DispatchableService::start()
	{
		// sanity check the service to make sure its included in context
		std::shared_ptr<IService> selfService;
		return serviceContext->getService(serviceName, selfService) && this == selfService.get();
	}

	void DispatchableService::stop()
	{
	}

	const KeyType DispatchableService::getKey() const
	{
		return dispatchKey;
	}
}
