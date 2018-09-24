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
	}

}


