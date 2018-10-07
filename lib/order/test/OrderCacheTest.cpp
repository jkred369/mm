/*
 * OrderCacheTest.cpp
 *
 *  Created on: Oct 6, 2018
 *      Author: suoalex
 */

#include <unordered_map>

#include <gtest/gtest.h>

#include <NullObjectPool.hpp>
#include <Order.hpp>
#include <OrderCache.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>
#include <PublisherAdapter.hpp>

namespace mm
{
	struct DummyPublisher : PublisherAdapter<OrderSummaryMessage>, PublisherAdapter<TradeMessage>
	{
		DummyPublisher(Dispatcher& dispatcher, QueueBasedObjectPool<OrderSummaryMessage>& pool, QueueBasedObjectPool<TradeMessage>& tradePool) :
			PublisherAdapter<OrderSummaryMessage> (dispatcher, pool),
			PublisherAdapter<TradeMessage> (dispatcher, tradePool)
		{
		}

		virtual const KeyType getKey() const override
		{
			return DispatchKey::ORDER;
		}

		virtual void publish(const Subscription& subscription, const OrderSummaryMessage* message) override
		{
			messageMap[subscription] = *message;
			PublisherAdapter<OrderSummaryMessage>::publish(subscription, message);
		}

		virtual void publish(const Subscription& subscription, const TradeMessage* message) override
		{
			tradeMap[subscription] = *message;
			PublisherAdapter<TradeMessage>::publish(subscription, message);
		}

		std::unordered_map<Subscription, OrderSummaryMessage> messageMap;

		std::unordered_map<Subscription, TradeMessage> tradeMap;
	};

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

	typedef Order<DummyExchange, QueueBasedObjectPool<OrderSummaryMessage>, QueueBasedObjectPool<TradeMessage> > TestOrder;

	TEST(OrderCacheTest, AddRemoveCase)
	{
		QueueBasedObjectPool<OrderSummaryMessage> summaryPool(10);
		QueueBasedObjectPool<TradeMessage> tradePool(10);

		Dispatcher dispatcher(2);
		DummyPublisher publisher(dispatcher, summaryPool, tradePool);
		DummyExchange exchange;

		TestOrder order(exchange, summaryPool, tradePool, publisher, publisher);

		{
			std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
			OrderMessage& message = *ptr;

			message.orderId = 1;
			message.instrumentId = 2;
			message.price = 20000.0;
			message.totalQty = 1;
			message.side = Side::BID;
			message.status = OrderStatus::LIVE;

			order.consume(ptr);

			ASSERT_TRUE(order.getOrderId() == message.orderId);
			ASSERT_TRUE(order.getInstrumentId() == message.instrumentId);
			ASSERT_TRUE(order.getPrice() == message.price);
			ASSERT_TRUE(order.getTotalQty() == message.totalQty);
			ASSERT_TRUE(order.getSide() == message.side);
			ASSERT_TRUE(order.getStatus() == OrderStatus::PENDING_ACK);

			ASSERT_TRUE(!exchange.statusMap.empty());
			ASSERT_TRUE(exchange.statusMap.find(message.orderId) != exchange.statusMap.end());

			ASSERT_TRUE(!publisher.messageMap.empty());

			{
				Subscription sub = {SourceType::ALL, DataType::ORDER_SUMMARY, message.orderId};
				ASSERT_TRUE(publisher.messageMap.find(sub) != publisher.messageMap.end());

				const OrderSummaryMessage& summary = publisher.messageMap[sub];

				ASSERT_TRUE(summary.orderId == message.orderId);
				ASSERT_TRUE(summary.instrumentId == message.instrumentId);
				ASSERT_TRUE(summary.price == message.price);
				ASSERT_TRUE(summary.totalQty == message.totalQty);
				ASSERT_TRUE(summary.side == message.side);
				ASSERT_TRUE(summary.status == OrderStatus::PENDING_ACK);
			}
		}

		OrderCache<TestOrder> cache;
		cache.addOrder(&order);

		{
			TestOrder* outOrder = cache.getOrder(2, 1);
			ASSERT_TRUE(outOrder);
			ASSERT_TRUE(outOrder == &order);
		}

		{
			TestOrder* outOrder = cache.removeOrder(2, 1);
			ASSERT_TRUE(outOrder);
			ASSERT_TRUE(outOrder == &order);
		}

		{
			TestOrder* outOrder = cache.removeOrder(2, 1);
			ASSERT_TRUE(!outOrder);
		}
	}

	TEST(OrderCacheTest, CrossCase)
	{
		QueueBasedObjectPool<OrderSummaryMessage> summaryPool(10);
		QueueBasedObjectPool<TradeMessage> tradePool(10);

		Dispatcher dispatcher(2);
		DummyPublisher publisher(dispatcher, summaryPool, tradePool);
		DummyExchange exchange;

		TestOrder order(exchange, summaryPool, tradePool, publisher, publisher);

		{
			std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
			OrderMessage& message = *ptr;

			message.orderId = 1;
			message.instrumentId = 2;
			message.price = 20000.0;
			message.totalQty = 1;
			message.side = Side::BID;
			message.status = OrderStatus::LIVE;

			order.consume(ptr);

			ASSERT_TRUE(order.getOrderId() == message.orderId);
			ASSERT_TRUE(order.getInstrumentId() == message.instrumentId);
			ASSERT_TRUE(order.getPrice() == message.price);
			ASSERT_TRUE(order.getTotalQty() == message.totalQty);
			ASSERT_TRUE(order.getSide() == message.side);
			ASSERT_TRUE(order.getStatus() == OrderStatus::PENDING_ACK);

			ASSERT_TRUE(!exchange.statusMap.empty());
			ASSERT_TRUE(exchange.statusMap.find(message.orderId) != exchange.statusMap.end());

			ASSERT_TRUE(!publisher.messageMap.empty());

			{
				Subscription sub = {SourceType::ALL, DataType::ORDER_SUMMARY, message.orderId};
				ASSERT_TRUE(publisher.messageMap.find(sub) != publisher.messageMap.end());

				const OrderSummaryMessage& summary = publisher.messageMap[sub];

				ASSERT_TRUE(summary.orderId == message.orderId);
				ASSERT_TRUE(summary.instrumentId == message.instrumentId);
				ASSERT_TRUE(summary.price == message.price);
				ASSERT_TRUE(summary.totalQty == message.totalQty);
				ASSERT_TRUE(summary.side == message.side);
				ASSERT_TRUE(summary.status == OrderStatus::PENDING_ACK);
			}
		}

		OrderCache<TestOrder> cache;
		cache.addOrder(&order);

		{
			TestOrder order2(exchange, summaryPool, tradePool, publisher, publisher);

			{
				std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
				OrderMessage& message = *ptr;

				message.orderId = 1;
				message.instrumentId = 2;
				message.price = 20000.0;
				message.totalQty = 1;
				message.side = Side::ASK;
				message.status = OrderStatus::LIVE;

				order2.consume(ptr);
			}

			ASSERT_TRUE(cache.hasCrossOrder(&order2));
		}

		{
			TestOrder order2(exchange, summaryPool, tradePool, publisher, publisher);

			{
				std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
				OrderMessage& message = *ptr;

				message.orderId = 1;
				message.instrumentId = 2;
				message.price = 19999.0;
				message.totalQty = 1;
				message.side = Side::ASK;
				message.status = OrderStatus::LIVE;

				order2.consume(ptr);
			}

			ASSERT_TRUE(cache.hasCrossOrder(&order2));
		}

		{
			TestOrder order2(exchange, summaryPool, tradePool, publisher, publisher);

			{
				std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
				OrderMessage& message = *ptr;

				message.orderId = 1;
				message.instrumentId = 2;
				message.price = 20001.0;
				message.totalQty = 1;
				message.side = Side::ASK;
				message.status = OrderStatus::LIVE;

				order2.consume(ptr);
			}

			ASSERT_TRUE(!cache.hasCrossOrder(&order2));
		}

		{
			TestOrder order2(exchange, summaryPool, tradePool, publisher, publisher);

			{
				std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
				OrderMessage& message = *ptr;

				message.orderId = 1;
				message.instrumentId = 2;
				message.price = 20001.0;
				message.totalQty = 1;
				message.side = Side::BID;
				message.status = OrderStatus::LIVE;

				order2.consume(ptr);
			}

			ASSERT_TRUE(!cache.hasCrossOrder(&order2));
		}
	}

}


