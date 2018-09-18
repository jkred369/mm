/*
 * PropertyFileConfig.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: suoalex
 */

#include <fstream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <StringUtil.hpp>
#include "PropertyConfig.hpp"

namespace mm
{
	PropertyConfig::PropertyConfig()
	{
	}

	PropertyConfig::PropertyConfig(std::istream is)
	{
		try
		{
			for (std::string line; std::getline(is, line); )
			{
				StringUtil::trim(line);
				if (line.size() == 0 || line[0] == '#')
				{
					continue;
				}

				const std::string::size_type index = line.find('=');
				if (index == std::string::npos || index == 0 || index == line.size() - 1)
				{
					LOGERR("Ignoring mal-formatted line: {}", line);
					continue;
				}

				const std::string key = line.substr(0, index);
				const std::string value = line.substr(index + 1);
				addValue(key, value);
			}
		}
		catch (std::exception& e)
		{
			LOGERR("Failed to read property: {}", e.what());
		}
		catch (...)
		{
			LOGERR("Failed to read property with unknown reason.");
		}
	}

	PropertyConfig::~PropertyConfig()
	{
	}

	bool PropertyConfig::getBool(const std::string& name) const
	{
	}

	void PropertyConfig::addValue(const std::string& key, const std::string& value)
	{
		const std::string::size_type index = key.find('.');

		// sanity check
		if (index == 0 || index == key.size() - 1)
		{
			LOGERR("Malformed property key: {}", key);
			return;
		}

		if (index == std::string::npos)
		{
			valueMap[key] = value;
		}
		else
		{
			const std::string subKey = key.substr(0, index);
			const std::string restKey = key.substr(index + 1);
			std::shared_ptr<PropertyConfig> subConfig = subConfigMap[subKey];

			if (!subConfig.get())
			{
				subConfig.reset(new PropertyConfig());
				subConfigMap[subKey] = subConfig;
			}

			subConfig->addValue(restKey, value);
		}
	}
}


