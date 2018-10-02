/*
 * RandomMarketDataSessionTest.cpp
 *
 *  Created on: Oct 2, 2018
 *      Author: suoalex
 */

#include <iostream>

#include <gtest/gtest.h>

#include <CountDownLatch.hpp>
#include <DispatchableService.hpp>
#include <MarketDataMessage.hpp>
#include <ProductService.hpp>
#include <StringBuffer.hpp>

#include "RandomMarketDataSession.hpp"

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

	class DummyAlgo :
			public DispatchableService,
			public IConsumer<Product>,
			public IConsumer<MarketDataMessage>
	{
	public:

		DummyAlgo(
				KeyType dispatchKey,
				const std::string& serviceName,
				ServiceContext& serviceContext,
				std::int64_t instrumentId,
				CountDownLatch<>& latch,
				std::size_t count) :
			DispatchableService(dispatchKey, serviceName, serviceContext),
			instrumentId(instrumentId),
			serviceContext(serviceContext),
			latch(latch),
			count(count),
			delaySum(0.0),
			receiveCount(0)
		{
		}

		virtual ~DummyAlgo()
		{
		}

		virtual bool start() override
		{
			{
				const Subscription sub = {SourceType::PRODUCT_SERVICE, DataType::PRODUCT, instrumentId};
				serviceContext.subscribe(sub, dynamic_cast<IConsumer<Product>*> (this));
			}

			{
				const Subscription sub = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, instrumentId};
				serviceContext.subscribe(sub, dynamic_cast<IConsumer<MarketDataMessage>*> (this));
			}

			return true;
		}

		virtual void stop() override
		{
			{
				const Subscription sub = {SourceType::PRODUCT_SERVICE, DataType::PRODUCT, instrumentId};
				serviceContext.unsubscribe(sub, dynamic_cast<IConsumer<Product>*> (this));
			}

			{
				const Subscription sub = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, instrumentId};
				serviceContext.unsubscribe(sub, dynamic_cast<IConsumer<MarketDataMessage>*> (this));
			}
		}

		virtual void consume(const std::shared_ptr<const MarketDataMessage>& message) override
		{
			RdtscTimer timer;

			receiveCount += 1;
			delaySum += timer.getTimeInNanos() - message->timestamp;

			if (receiveCount == count)
			{
				latch.countDown();
			}
		}

		virtual void consume(const std::shared_ptr<const Product>& message) override
		{
			std::cout << "Product received." << std::endl;
		}

		double getDelaySum() const
		{
			return delaySum;
		}

		std::size_t getCount() const
		{
			return receiveCount;
		}

	private:

		const std::int64_t instrumentId;

		ServiceContext& serviceContext;

		CountDownLatch<>& latch;

		const std::size_t count;

		double delaySum;

		std::size_t receiveCount;
	};

	class DummyServiceContext : public ServiceContext
	{
	public:

		DummyServiceContext(std::istream&& is, IServiceFactory& factory) : ServiceContext(is, factory)
		{
		}

		bool setService(const std::string& serviceName, const std::shared_ptr<IService>& service)
		{
			return ServiceContext::setService(serviceName, service);
		}
	};

	TEST(RandomMarketDataSessionTest, PerformanceCase)
	{
		DummyFactory factory;
		DummyServiceContext serviceContext(std::stringstream(""), factory);

		std::stringstream ss;
		{
			StringBuffer buffer;

			ProductMessage message;
			message.id = 2;
			message.underlyerId = 0;
			message.symbol = "IF1503";
			message.exchange = Exchange::SHFE;
			message.productType = ProductType::FUTURE;
			message.currency = Currency::CNY;
			message.callPut = CallPutType::CALL;
			message.lotSize = 1;

			message.serialize(buffer);
			ss << buffer;
		}

		CountDownLatch<> latch(1);
		const std::size_t count = 1000;

		std::shared_ptr<ProductService> service(new ProductService(1, "DummyProductService", serviceContext, ss));
		ASSERT_TRUE(serviceContext.setService("DummyProductService", service));

		std::shared_ptr<DummyAlgo> algo(new DummyAlgo(0, "DummyAlgo", serviceContext, 2, latch, count));
		ASSERT_TRUE(serviceContext.setService("DummyAlgo", service));

		ASSERT_TRUE(serviceContext.start());

		// wait until message done then collect result
		latch.wait();

		// tear down
		serviceContext.stop();
		std::cout << "Market data to consume delay (ns): " << algo->getDelaySum() / algo->getCount() << std::endl;
	}

}
