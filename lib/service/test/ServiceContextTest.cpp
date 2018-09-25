/*
 * ServiceContextTest.cpp
 *
 *  Created on: Sep 25, 2018
 *      Author: suoalex
 */

#include <cstdint>
#include <functional>
#include <memory>

#include <gtest/gtest.h>

#include "ServiceContext.hpp"

namespace mm
{
	struct DummyFactory : IServiceFactory
	{
		virtual ~DummyFactory()
		{
		}

		virtual std::shared_ptr<IService> createService(
				const std::string serviceClass,
				const std::shared_ptr<IConfig> config,
				ServiceContext& context) override
		{
			return std::shared_ptr<IService> ();
		}
	};

	TEST(ServiceContextTest, DummyCase)
	{
		DummyFactory factory;

		std::stringstream ss("");

		// should throw exception about no dispatcher defined
		try
		{
			ServiceContext context(ss, factory);
			ASSERT_TRUE(false);
		}
		catch (const std::exception& e)
		{
		}
	}

}






