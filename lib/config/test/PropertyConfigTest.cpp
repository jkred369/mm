/*
 * PropertyConfigTest.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: suoalex
 */

#include <cstdint>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include <gtest/gtest.h>

#include "PropertyConfig.hpp"

namespace mm
{
	TEST(PropertyConfigTest, DummyCase)
	{
		PropertyConfig config;

		{
			bool caught = true;
			try
			{
				std::string name("ABC");
				config.getBool(name);
			}
			catch (std::runtime_error& e)
			{
				caught = true;
			}

			ASSERT_TRUE(caught);
		}

		{
			bool caught = true;
			try
			{
				std::string name("DEF");
				config.getDouble(name);
			}
			catch (std::runtime_error& e)
			{
				caught = true;
			}

			ASSERT_TRUE(caught);
		}

		{
			bool caught = true;
			try
			{
				std::string name("GHI");
				config.getInt32(name);
			}
			catch (std::runtime_error& e)
			{
				caught = true;
			}

			ASSERT_TRUE(caught);
		}

		{
			bool caught = true;
			try
			{
				std::string name("JKL");
				config.getInt64(name);
			}
			catch (std::runtime_error& e)
			{
				caught = true;
			}

			ASSERT_TRUE(caught);
		}

		{
			bool caught = true;
			try
			{
				std::string name("MNO");
				config.getString(name);
			}
			catch (std::runtime_error& e)
			{
				caught = true;
			}

			ASSERT_TRUE(caught);
		}

		ASSERT_TRUE(config.getBool("ABC", true));
		ASSERT_TRUE(config.getDouble("ABC", 12.5) == 12.5);
		ASSERT_TRUE(config.getInt32("ABC", 1234) == 1234);
		ASSERT_TRUE(config.getInt64("ABC", 123456) == 123456);
		ASSERT_TRUE(config.getString("ABC", "DEF") == "DEF");

		ASSERT_TRUE(config.getBoolList("ABC").size() == 0);
		ASSERT_TRUE(config.getDoubleList("ABC").size() == 0);
		ASSERT_TRUE(config.getInt32List("ABC").size() == 0);
		ASSERT_TRUE(config.getInt64List("ABC").size() == 0);

		ASSERT_TRUE(config.getSubConfig("ABC").get() == nullptr);
	}

	TEST(PropertyConfigTest, BasicCase)
	{
		std::stringstream ss;
		ss << "# A testing property config" << std::endl;
		ss << "" << std::endl;
		ss << "Services = Order,FemasMarketData, Dispatcher" << std::endl;
		ss << "Order.Count = 0" << std::endl;
		ss << " Order . Bootstrap = 1" << std::endl;
		ss << " Order.Exchange = 2" << std::endl;
		ss << "Order.Exchange.Venue = Femas" << std::endl;
		ss << "FemasMarketData.Instruments=1,2,3,4" << std::endl;
		ss << "FemasMarketData.Limits=1.2,2.1,3.3,4.0" << std::endl;
		ss << "FemasMarketData.Names=Femas , CTP , Bottom" << std::endl;
		ss << "Dispatcher.Thread=2" << std::endl;

		PropertyConfig config(ss);

		{
			std::vector<std::string> values = config.getStringList("Services");
			ASSERT_TRUE(values.size() == 3);
			ASSERT_TRUE(values[0] == "Order");
			ASSERT_TRUE(values[1] == "FemasMarketData");
			ASSERT_TRUE(values[2] == "Dispatcher");
		}

		{
			std::shared_ptr<IConfig> orderConfig = config.getSubConfig("Order");
			ASSERT_TRUE(orderConfig->getBool("Bootstrap"));
			ASSERT_TRUE(orderConfig->getInt64("Exchange") == 2);

			std::shared_ptr<IConfig> exchangeConfig = orderConfig->getSubConfig("Exchange");
			ASSERT_TRUE(exchangeConfig->getString("Venue", "") == "Femas");
		}

		{
			std::shared_ptr<IConfig> femasConfig = config.getSubConfig("FemasMarketData");

			std::vector<std::int64_t> instruments = femasConfig->getInt64List("Instruments");
			ASSERT_TRUE(instruments.size() == 4);
			ASSERT_TRUE(instruments[0] == 1);
			ASSERT_TRUE(instruments[1] == 2);
			ASSERT_TRUE(instruments[2] == 3);
			ASSERT_TRUE(instruments[3] == 4);

			std::vector<double> limits = femasConfig->getDoubleList("Limits");
			ASSERT_TRUE(limits.size() == 4);
			ASSERT_TRUE(limits[0] == 1.2);
			ASSERT_TRUE(limits[1] == 2.1);
			ASSERT_TRUE(limits[2] == 3.3);
			ASSERT_TRUE(limits[3] == 4.0);

			std::vector<std::string> names = femasConfig->getStringList("Names");
			ASSERT_TRUE(names.size() == 3);
			ASSERT_TRUE(names[0] == "Femas");
			ASSERT_TRUE(names[1] == "CTP");
			ASSERT_TRUE(names[2] == "Bottom");
		}

		{
			std::shared_ptr<IConfig> dispatcherConfig = config.getSubConfig("Dispatcher");
			ASSERT_TRUE(dispatcherConfig->getInt32("Thread", 1) == 2);
		}
	}

}



