/*
 * PropertyFileConfig.hpp
 *
 *  Created on: Sep 17, 2018
 *      Author: suoalex
 */

#ifndef LIB_CONFIG_SRC_PROPERTYFILECONFIG_HPP_
#define LIB_CONFIG_SRC_PROPERTYFILECONFIG_HPP_

#include <IConfig.hpp>

namespace mm
{
	class PropertyFileConfig : public IConfig
	{
	public:

		//
		// Constructor.
		//
		// path : The path to the configuration file.
		//
		PropertyFileConfig(const std::string path);

		virtual ~PropertyFileConfig();

		// all interface methods from IConfig
		virtual const IConfig* getSubConfig(const std::string& name) const override;

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
	};
}

#endif /* LIB_CONFIG_SRC_PROPERTYFILECONFIG_HPP_ */
