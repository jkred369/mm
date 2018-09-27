/*
 * ServiceContextTest.cpp
 *
 *  Created on: Sep 25, 2018
 *      Author: suoalex
 */

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

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

	protected:

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

	struct RunningService : SimpleService
	{
		RunningService(const std::string className, Dispatcher& dispatcher) : SimpleService(className), dispatcher(dispatcher)
		{
		}

		virtual ~RunningService()
		{
		}

		virtual bool start() override
		{
			if (!SimpleService::start())
			{
				return false;
			}

			const KeyType key = className == "A" ? 1 : 2;
			dispatcher.submit(key, [&] () {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				std::cout << "running" << std::endl;
				++this->counter;
			});

			std::cout << "inserted" << std::endl;

			return true;
		}

		virtual void stop() override
		{
			--counter;
		}

	private:

		Dispatcher& dispatcher;
	};

	struct RunningFactory : IServiceFactory
	{
		virtual ~RunningFactory()
		{
		}

		virtual std::shared_ptr<IService> createService(
				const std::string serviceClass,
				const std::shared_ptr<IConfig> config,
				ServiceContext& context) override
		{
			return std::shared_ptr<IService> (new RunningService(serviceClass, context.getDispatcher()));
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

		SimpleService* service1 = nullptr;
		ASSERT_TRUE(context.getService("Service1", service1));
		ASSERT_TRUE(service1);
		ASSERT_TRUE(service1->getClass() == "A");

		SimpleService* service2 = nullptr;
		ASSERT_TRUE(context.getService("Service2", service2));
		ASSERT_TRUE(service2);
		ASSERT_TRUE(service2->getClass() == "B");

		// service start
		ASSERT_TRUE(context.start());
		ASSERT_TRUE(service1->getCount() == 1);
		ASSERT_TRUE(service2->getCount() == 1);

		context.stop();
		ASSERT_TRUE(service1->getCount() == 0);
		ASSERT_TRUE(service2->getCount() == 0);
	}

	TEST(ServiceContextTest, RunningServiceCase)
	{
		RunningFactory factory;

		std::stringstream ss("");
		ss << "Dispatcher.ThreadCount=2" << std::endl;
		ss << "ServiceList=Service1,Service2" << std::endl;
		ss << "Service1.Class=A" << std::endl;
		ss << "Service2.Class=B" << std::endl;

		// service creation
		ServiceContext context(ss, factory);

		RunningService* service1 = nullptr;
		ASSERT_TRUE(context.getService("Service1", service1));
		ASSERT_TRUE(service1);
		ASSERT_TRUE(service1->getClass() == "A");

		RunningService* service2 = nullptr;
		ASSERT_TRUE(context.getService("Service2", service2));
		ASSERT_TRUE(service2);
		ASSERT_TRUE(service2->getClass() == "B");

		// service start
		ASSERT_TRUE(context.start());
		ASSERT_TRUE(service1->getCount() == 1);
		ASSERT_TRUE(service2->getCount() == 1);

		std::this_thread::sleep_for(std::chrono::milliseconds(4));
		ASSERT_TRUE(service1->getCount() == 2);
		ASSERT_TRUE(service2->getCount() == 2);

		context.stop();
		ASSERT_TRUE(service1->getCount() == 1);
		ASSERT_TRUE(service2->getCount() == 1);
	}

}

