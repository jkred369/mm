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
			const std::shared_ptr<Dispatcher> dispatcher,
			const std::shared_ptr<ServiceContext> serviceContext) :
		dispatchKey(dispatchKey),
		dispatcher(dispatcher),
		serviceContext(serviceContext)
	{
	}

	bool DispatchableService::start()
	{
		return true;
	}

	void DispatchableService::stop()
	{
	}

	const KeyType DispatchableService::getKey() const
	{
		return dispatchKey;
	}
}
