/*
 * PropertyFileConfig.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: suoalex
 */

#include <fstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <StringUtil.hpp>
#include "PropertyConfig.hpp"

namespace mm
{
	PropertyConfig::PropertyConfig()
	{
	}

	PropertyConfig::PropertyConfig(std::istream& is)
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

	const std::shared_ptr<IConfig> PropertyConfig::getSubConfig(const std::string& name) const
	{
		auto it = subConfigMap.find(name);
		return it == subConfigMap.end() ? std::shared_ptr<IConfig>() : std::static_pointer_cast<IConfig>(it->second);
	}

	bool PropertyConfig::getBool(const std::string& name) const
	{
		const std::string& value = getString(name);

		try
		{
			return boost::lexical_cast<bool> (value);
		}
		catch (const boost::bad_lexical_cast& e)
		{
			LOGERR("Error casting value {} to bool on key {}", value, name);
			throw e;
		}
	}

	bool PropertyConfig::getBool(const std::string& name, bool defaultValue) const
	{
		return valueMap.find(name) == valueMap.end() ? defaultValue : getBool(name);
	}

	const std::vector<bool> PropertyConfig::getBoolList(const std::string& name) const
	{
		std::vector<bool> result;
		const std::vector<std::string> items = getStringList(name);

		for (const std::string& item : items)
		{
			try
			{
				result.push_back(boost::lexical_cast<bool> (item));
			}
			catch (const boost::bad_lexical_cast& e)
			{
				LOGERR("Error casting item {} to bool on key {}", item, name);
			}
		}

		return result;
	}

	std::int32_t PropertyConfig::getInt32(const std::string& name) const
	{
		const std::string& value = getString(name);

		try
		{
			return boost::lexical_cast<std::int32_t> (value);
		}
		catch (const boost::bad_lexical_cast& e)
		{
			LOGERR("Error casting valye {} to int32 on key {}", value, name);
			throw e;
		}
	}

	std::int32_t PropertyConfig::getInt32(const std::string& name, std::int32_t defaultValue) const
	{
		return valueMap.find(name) == valueMap.end() ? defaultValue : getInt32(name);
	}

	const std::vector<std::int32_t> PropertyConfig::getInt32List(const std::string& name) const
	{
		std::vector<std::int32_t> result;
		const std::vector<std::string> items = getStringList(name);

		for (const std::string& item : items)
		{
			try
			{
				result.push_back(boost::lexical_cast<std::int32_t> (item));
			}
			catch (const boost::bad_lexical_cast& e)
			{
				LOGERR("Error casting item {} to int32 on key {}", item, name);
			}
		}

		return result;
	}

	std::int64_t PropertyConfig::getInt64(const std::string& name) const
	{
		const std::string& value = getString(name);

		try
		{
			return boost::lexical_cast<std::int64_t> (value);
		}
		catch (const boost::bad_lexical_cast& e)
		{
			LOGERR("Error casting valye {} to int64 on key {}", value, name);
			throw e;
		}
	}

	std::int64_t PropertyConfig::getInt64(const std::string& name, std::int64_t defaultValue) const
	{
		return valueMap.find(name) == valueMap.end() ? defaultValue : getInt64(name);
	}

	const std::vector<std::int64_t> PropertyConfig::getInt64List(const std::string& name) const
	{
		std::vector<std::int64_t> result;
		const std::vector<std::string> items = getStringList(name);

		for (const std::string& item : items)
		{
			try
			{
				result.push_back(boost::lexical_cast<std::int64_t> (item));
			}
			catch (const boost::bad_lexical_cast& e)
			{
				LOGERR("Error casting item {} to int64 on key {}", item, name);
			}
		}

		return result;
	}

	double PropertyConfig::getDouble(const std::string& name) const
	{
		const std::string& value = getString(name);

		try
		{
			return boost::lexical_cast<double> (value);
		}
		catch (const boost::bad_lexical_cast& e)
		{
			LOGERR("Error casting valye {} to double on key {}", value, name);
			throw e;
		}
	}

	double PropertyConfig::getDouble(const std::string& name, double defaultValue) const
	{
		return valueMap.find(name) == valueMap.end() ? defaultValue : getDouble(name);
	}

	const std::vector<double> PropertyConfig::getDoubleList(const std::string& name) const
	{
		std::vector<double> result;
		const std::vector<std::string> items = getStringList(name);

		for (const std::string& item : items)
		{
			try
			{
				result.push_back(boost::lexical_cast<double> (item));
			}
			catch (const boost::bad_lexical_cast& e)
			{
				LOGERR("Error casting item {} to double on key {}", item, name);
			}
		}

		return result;
	}

	const std::string& PropertyConfig::getString(const std::string& name) const
	{
		auto it = valueMap.find(name);
		if (it == valueMap.end())
		{
			throw std::runtime_error("Cannot find config with key: " + name);
		}

		return it->second;
	}

	const std::string& PropertyConfig::getString(const std::string& name, const std::string& defaultValue) const
	{
		auto it = valueMap.find(name);
		return it == valueMap.end() ? defaultValue : it->second;
	}

	const std::vector<std::string> PropertyConfig::getStringList(const std::string& name) const
	{
		std::vector<std::string> result;

		const std::string& value = getString(name, "");
		if (value != "")
		{
			boost::split(result, value, boost::is_any_of(","));
			for (std::string& value : result)
			{
				StringUtil::trim(value);
			}
		}

		return result;
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


