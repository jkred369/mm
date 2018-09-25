/*
 * ServiceContextTest.cpp
 *
 *  Created on: Sep 25, 2018
 *      Author: suoalex
 */

#include <atomic>
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

	struct SimpleService : IService
	{
		SimpleService(const std::string className) : className(className)
		{
		}

		virtual ~SimpleService()
		{
		}

		virtual bool start() override
		{
			++counter;
			return true;
		}

		virtual void stop() override
		{
			--counter;
		}

		const std::string& getClass() const
		{
			return className;
		}

		int getCount() const
		{
			return counter.load();
		}

	private:

		const std::string className;

		std::atomic<int> counter;
	};

	struct SimpleFactory : IServiceFactory
	{
		virtual ~SimpleFactory()
		{
		}

		virtual std::shared_ptr<IService> createService(
				const std::string serviceClass,
				const std::shared_ptr<IConfig> config,
				ServiceContext& context) override
		{
			return std::shared_ptr<IService> (new SimpleService(serviceClass));
		}
	};

	TEST(ServiceContextTest, DummyCase)
	{
		DummyFactory factory;
		ServiceContext context(std::stringstream(""), factory);

		ASSERT_TRUE(context.start());
		context.stop();
	}

	TEST(ServiceContextTest, DispatcherCase)
	{
		DummyFactory factory;
		ServiceContext context(std::stringstream("Dispatcher.ThreadCount=2"), factory);

		ASSERT_TRUE(context.start());
		context.stop();
	}

	TEST(ServiceContextTest, SimpleServiceCase)
	{
		SimpleFactory factory;

		std::stringstream ss("");
		ss << "Dispatcher.ThreadCount=2" << std::endl;
		ss << "ServiceList=Service1,Service2" << std::endl;
		ss << "Service1.Class=A" << std::endl;
		ss << "Service2.Class=B" << std::endl;

		// service creation
		ServiceContext context(ss, factory);

		std::shared_ptr<SimpleService> service1;
		ASSERT_TRUE(context.getService("Service1", service1));
		ASSERT_TRUE(service1.get());
		ASSERT_TRUE(service1->getClass() == "A");

		std::shared_ptr<SimpleService> service2;
		ASSERT_TRUE(context.getService("Service2", service2));
		ASSERT_TRUE(service2.get());
		ASSERT_TRUE(service2->getClass() == "B");

		// service start
		ASSERT_TRUE(context.start());
		ASSERT_TRUE(service1->getCount() == 1);
		ASSERT_TRUE(service2->getCount() == 1);

		context.stop();
		ASSERT_TRUE(service1->getCount() == 0);
		ASSERT_TRUE(service2->getCount() == 0);
	}

}

