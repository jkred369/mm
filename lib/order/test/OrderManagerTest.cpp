/*
 * OrderManagerTest.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: suoalex
 */

#include <chrono>
#include <thread>
#include <unordered_map>

#include <gtest/gtest.h>

#include <NullObjectPool.hpp>
#include <OrderManager.hpp>

namespace mm
{
	struct DummyExchange
	{
		void sendOrder(const std::shared_ptr<const OrderMessage>& message)
		{
			statusMap[message->orderId] = OrderStatus::LIVE;
		}

		void cancel(const std::shared_ptr<const OrderMessage>& message)
		{
			auto it = statusMap.find(message->orderId);
			ASSERT_TRUE(it != statusMap.end());

			it->second = OrderStatus::CANCELLED;
		}

		std::unordered_map<std::int64_t, OrderStatus> statusMap;
	};

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
			public IConsumer<OrderSummaryMessage>,
			public IConsumer<TradeMessage>
	{
	public:

		DummyAlgo(
				KeyType dispatchKey,
				const std::string& serviceName,
				ServiceContext& serviceContext,
				std::int64_t instrumentId) :
			DispatchableService(dispatchKey, serviceName, serviceContext),
			instrumentId(instrumentId),
			serviceContext(serviceContext)
		{
		}

		virtual ~DummyAlgo()
		{
		}

		virtual bool start() override
		{
			if (!DispatchableService::start())
			{
				return false;
			}

			{
				const Subscription sub = {SourceType::ALL, DataType::ORDER_SUMMARY, instrumentId};
				serviceContext.subscribe(sub, dynamic_cast<IConsumer<OrderSummaryMessage>*> (this));
			}

			{
				const Subscription sub = {SourceType::ALL, DataType::TRADE, instrumentId};
				serviceContext.subscribe(sub, dynamic_cast<IConsumer<TradeMessage>*> (this));
			}

			return true;
		}

		virtual void stop() override
		{
			{
				const Subscription sub = {SourceType::ALL, DataType::ORDER_SUMMARY, instrumentId};
				serviceContext.unsubscribe(sub, dynamic_cast<IConsumer<OrderSummaryMessage>*> (this));
			}

			{
				const Subscription sub = {SourceType::ALL, DataType::TRADE, instrumentId};
				serviceContext.unsubscribe(sub, dynamic_cast<IConsumer<TradeMessage>*> (this));
			}

			DispatchableService::stop();
		}

		virtual void consume(const std::shared_ptr<const OrderSummaryMessage>& message) override
		{
			summaryMap[message->orderId] = message;
		}

		virtual void consume(const std::shared_ptr<const TradeMessage>& message) override
		{

		}

		std::unordered_map<std::int64_t, std::shared_ptr<const OrderSummaryMessage> > summaryMap;

	private:

		const std::int64_t instrumentId;

		ServiceContext& serviceContext;
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

	typedef OrderManager<DummyExchange, NullObjectPool> TestOrderManager;

	TEST(OrderManagerTest, SendCancelCase)
	{
		DummyExchange exchange;

		DummyFactory factory;
		DummyServiceContext serviceContext(std::stringstream(""), factory);

		std::shared_ptr<DummyAlgo> dummyAlgo(new DummyAlgo(DispatchKey::ALGO, "DummyAlgo", serviceContext, 2));
		serviceContext.setService("DummyAlgo", dummyAlgo);

		std::shared_ptr<TestOrderManager> orderManager(new TestOrderManager(DispatchKey::ORDER, "OrderManager", serviceContext, exchange));
		serviceContext.setService("OrderManager", orderManager);

		serviceContext.start();

		// new order and send
		{
			std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
			OrderMessage& message = *ptr;

			message.orderId = 1;
			message.instrumentId = 2;
			message.price = 20000.0;
			message.totalQty = 1;
			message.side = Side::BID;
			message.status = OrderStatus::LIVE;

			orderManager->consume(ptr);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			ASSERT_TRUE(!exchange.statusMap.empty());
			ASSERT_TRUE(exchange.statusMap.find(message.orderId) != exchange.statusMap.end());

			ASSERT_TRUE(!dummyAlgo->summaryMap.empty());

			{
				ASSERT_TRUE(dummyAlgo->summaryMap.find(message.orderId) != dummyAlgo->summaryMap.end());

				std::shared_ptr<const OrderSummaryMessage> summaryPtr = dummyAlgo->summaryMap[message.orderId];
				ASSERT_TRUE(summaryPtr);

				const OrderSummaryMessage& summary = *summaryPtr;
				ASSERT_TRUE(summary.orderId == message.orderId);
				ASSERT_TRUE(summary.instrumentId == message.instrumentId);
				ASSERT_TRUE(summary.price == message.price);
				ASSERT_TRUE(summary.totalQty == message.totalQty);
				ASSERT_TRUE(summary.side == message.side);
				ASSERT_TRUE(summary.status == OrderStatus::PENDING_ACK);
			}
		}

		serviceContext.stop();
	}
}
