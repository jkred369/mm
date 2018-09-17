/*
 * IConfig.hpp
 *
 *  Created on: Sep 17, 2018
 *      Author: suoalex
 */

#ifndef LIB_CONFIG_SRC_ICONFIG_HPP_
#define LIB_CONFIG_SRC_ICONFIG_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace mm
{
	//
	// This class defines the interface for configuration.
	//
	class IConfig
	{
	public:

		// Virtual destructor.
		virtual ~IConfig() {}

		//
		// Get the sub config from the current level config.
		//
		// return : pointer to the sub config; or nullptr if config with the give name cannot be found.
		//
		virtual const IConfig* getSubConfig(const std::string& name) const = 0;

		//
		// Get the config on the given name as a bool value. If the key cannot be found throw exception.
		//
		// name : The config name.
		//
		// return : The config as bool value.
		//
		virtual bool getBool(const std::string& name) const = 0;

		//
		// Get the config on the give name as a bool value. If the key cannot be found return the default value.
		//
		// name : The config name.
		// defaultValue : The default value.
		//
		// return : The config as bool value or the default value if cannot be found.
		//
		virtual bool getBool(const std::string& name, bool defaultValue) const = 0;

		//
		// Get the config on the give name as a bool vector. If the key cannot be found return empty vector.
		//
		// name : The config name.
		//
		// return : The config as bool vector or empty vector if cannot be found.
		//
		virtual const std::vector<bool> getBoolList(const std::string& name) const = 0;

		//
		// Get the config on the given name as a int32 value. If the key cannot be found throw exception.
		//
		// name : The config name.
		//
		// return : The config as int32 value.
		//
		virtual std::int32_t getInt32(const std::string& name) const = 0;

		//
		// Get the config on the give name as a int32 value. If the key cannot be found return the default value.
		//
		// name : The config name.
		// defaultValue : The default value.
		//
		// return : The config as int32 value or the default value if cannot be found.
		//
		virtual std::int32_t getInt32(const std::string& name, std::int32_t defaultValue) const = 0;

		//
		// Get the config on the give name as a int32 vector. If the key cannot be found return empty vector.
		//
		// name : The config name.
		//
		// return : The config as int32 vector or empty vector if cannot be found.
		//
		virtual const std::vector<std::int32_t> getInt32List(const std::string& name) const = 0;

		//
		// Get the config on the given name as a int64 value. If the key cannot be found throw exception.
		//
		// name : The config name.
		//
		// return : The config as int64 value.
		//
		virtual std::int64_t getInt64(const std::string& name) const = 0;

		//
		// Get the config on the give name as a bool value. If the key cannot be found return the default value.
		//
		// name : The config name.
		// defaultValue : The default value.
		//
		// return : The config as bool value or the default value if cannot be found.
		//
		virtual std::int64_t getInt64(const std::string& name, std::int64_t defaultValue) const = 0;

		//
		// Get the config on the give name as a int64 vector. If the key cannot be found return empty vector.
		//
		// name : The config name.
		//
		// return : The config as int64 vector or empty vector if cannot be found.
		//
		virtual const std::vector<std::int64_t> getInt64List(const std::string& name) const = 0;

		//
		// Get the config on the given name as a double value. If the key cannot be found throw exception.
		//
		// name : The config name.
		//
		// return : The config as double value.
		//
		virtual double getDouble(const std::string& name) const = 0;

		//
		// Get the config on the give name as a double value. If the key cannot be found return the default value.
		//
		// name : The config name.
		// defaultValue : The default value.
		//
		// return : The config as double value or the default value if cannot be found.
		//
		virtual double getDouble(const std::string& name, double defaultValue) const = 0;

		//
		// Get the config on the give name as a double vector. If the key cannot be found return empty vector.
		//
		// name : The config name.
		//
		// return : The config as double vector or empty vector if cannot be found.
		//
		virtual const std::vector<double> getDoubleList(const std::string& name) const = 0;

		//
		// Get the config on the given name as a string value. If the key cannot be found throw exception.
		//
		// name : The config name.
		//
		// return : The config as string value.
		//
		virtual const std::string& getString(const std::string& name) const = 0;

		//
		// Get the config on the give name as a string value. If the key cannot be found return the default value.
		//
		// name : The config name.
		// defaultValue : The default value.
		//
		// return : The config as string value or the default value if cannot be found.
		//
		virtual const std::string& getString(const std::string& name, const std::string& defaultValue) const = 0;

		//
		// Get the config on the give name as a string vector. If the key cannot be found return empty vector.
		//
		// name : The config name.
		//
		// return : The config as string vector or empty vector if cannot be found.
		//
		virtual const std::vector<std::string> getStringList(const std::string& name) const = 0;

	};
}


#endif /* LIB_CONFIG_SRC_ICONFIG_HPP_ */
