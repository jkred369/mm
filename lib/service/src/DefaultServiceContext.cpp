/*
 * DefaultServiceContext.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#include <Log.hpp>

#include "DefaultServiceContext.hpp"

namespace mm
{
	DefaultServiceContext::~DefaultServiceContext()
	{
		for (std::pair<std::string, std::shared_ptr<IService> >& value : serviceMap)
		{
			LOGINFO << "Stopping service " << pair.first << ENDLOG;
			value.second->stop();
		}

		LOGINFO << "Shutting down context" << ENDLOG;
		serviceMap.clear();
	}

	IService* DefaultServiceContext::getService(const std::string& serviceName)
	{
		auto it = serviceMap.find(serviceName);
		return it == serviceMap.end() ? nullptr : it->second.get();
	}
}
