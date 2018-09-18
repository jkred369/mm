/*
 * ServiceContext.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#include <fstream>

#include <PropertyConfig.hpp>

#include "ServiceContext.hpp"

namespace mm
{
	ServiceContext::ServiceContext(const std::string configFilePath, IServiceFactory& factory)
	{
		// initialize the config
		try
		{
			std::ifstream ifs(configFilePath);
			config.reset(new PropertyConfig(ifs));

			LOGINFO("Configuration loaded from file: {}", configFilePath);
		}
		catch (const std::exception& e)
		{
			LOGFATAL("Error loading configuration file: {}", e.what());
		}

		// create fundamental components


		// create services

	}
}


