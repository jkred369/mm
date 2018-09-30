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
			const std::string serviceName,
			ServiceContext& serviceContext) :
		dispatchKey(dispatchKey),
		serviceName(serviceName),
		serviceContext(serviceContext)
	{
	}

	bool DispatchableService::start()
	{
		// sanity check the service to make sure its included in context
		IService* selfService = nullptr;
		return serviceContext.getService(serviceName, selfService) && this == selfService;
	}

	void DispatchableService::stop()
	{
	}

	const KeyType DispatchableService::getKey() const
	{
		return dispatchKey;
	}
}
