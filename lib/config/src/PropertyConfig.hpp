/*
 * PropertyFileConfig.hpp
 *
 *  Created on: Sep 17, 2018
 *      Author: suoalex
 */

#ifndef LIB_CONFIG_SRC_PROPERTYCONFIG_HPP_
#define LIB_CONFIG_SRC_PROPERTYCONFIG_HPP_

#include <istream>
#include <unordered_map>

#include <Logger.hpp>
#include <IConfig.hpp>

namespace mm
{
	//
	// A simple property file based config.
	//
	// - all lines starting with '#' as first non-trivial char will be ignored
	// - all property as {A}.{B}.{C} will be treated as sub configurations.
	// - all ',' separated strings will be treated as list of values.
	//
	class PropertyConfig : public IConfig
	{
	public:

		//
		// Default constructor.
		//
		PropertyConfig();

		//
		// Constructor.
		//
		// is : The input stream.
		//
		PropertyConfig(std::istream is);

		virtual ~PropertyConfig();

		// all interface methods from IConfig
		virtual const std::shared_ptr<IConfig> getSubConfig(const std::string& name) const override;

		virtual bool getBool(const std::string& name) const override;
		virtual bool getBool(const std::string& name, bool defaultValue) const override;
		virtual const std::vector<bool> getBoolList(const std::string& name) const override;

		virtual std::int32_t getInt32(const std::string& name) const override;
		virtual std::int32_t getInt32(const std::string& name, std::int32_t defaultValue) const override;
		virtual const std::vector<std::int32_t> getInt32List(const std::string& name) const override;

		virtual std::int64_t getInt64(const std::string& name) const override;
		virtual std::int64_t getInt64(const std::string& name, std::int64_t defaultValue) const override;
		virtual const std::vector<std::int64_t> getInt64List(const std::string& name) const override;

		virtual double getDouble(const std::string& name) const override;
		virtual double getDouble(const std::string& name, double defaultValue) const override;
		virtual const std::vector<double> getDoubleList(const std::string& name) const override;

		virtual const std::string& getString(const std::string& name) const override;
		virtual const std::string& getString(const std::string& name, const std::string& defaultValue) const override;
		virtual const std::vector<std::string> getStringList(const std::string& name) const override;

	private:

		//
		// Add a value to the config with the given key. If the key represents sub config then the corresponding
		// structure should be established.
		//
		// key : The key with potentially subconfig structure as {A}.{B}.{C}
		// value : The value.
		//
		void addValue(const std::string& key, const std::string& value);

		// The logger for this class.
		static Logger logger;

		// The value map where key is the value name.
		std::unordered_map<std::string, std::string> valueMap;

		// The sub config map where key is the sub config group name.
		std::unordered_map<std::string, std::shared_ptr<PropertyConfig> > subConfigMap;
	};
}

#endif /* LIB_CONFIG_SRC_PROPERTYCONFIG_HPP_ */
