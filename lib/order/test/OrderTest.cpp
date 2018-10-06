/*
 * OrderTest.cpp
 *
 *  Created on: Oct 4, 2018
 *      Author: suoalex
 */

#include <unordered_map>

#include <gtest/gtest.h>

#include <NullObjectPool.hpp>
#include <Order.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>
#include <PublisherAdapter.hpp>

namespace mm
{
	struct DummyPublisher : PublisherAdapter<OrderSummaryMessage>, PublisherAdapter<TradeMessage>
	{
		DummyPublisher(Dispatcher& dispatcher) :
			PublisherAdapter<OrderSummaryMessage> (dispatcher),
			PublisherAdapter<TradeMessage> (dispatcher)
		{
		}

		virtual const KeyType getKey() const override
		{
			return DispatchKey::ORDER;
		}

		virtual void publish(const Subscription& subscription, const std::shared_ptr<const OrderSummaryMessage>& message) override
		{
			messageMap[subscription] = message;
			PublisherAdapter<OrderSummaryMessage>::publish(subscription, message);
		}

		virtual void publish(const Subscription& subscription, const std::shared_ptr<const TradeMessage>& message) override
		{
			tradeMap[subscription] = message;
			PublisherAdapter<TradeMessage>::publish(subscription, message);
		}

		std::unordered_map<Subscription, std::shared_ptr<const OrderSummaryMessage> > messageMap;

		std::unordered_map<Subscription, std::shared_ptr<const TradeMessage> > tradeMap;
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

	typedef Order<DummyExchange, NullObjectPool<OrderSummaryMessage>, NullObjectPool<TradeMessage> > TestOrder;

	TEST(OrderTest, SendCase)
	{
		Dispatcher dispatcher(2);
		DummyPublisher publisher(dispatcher);
		DummyExchange exchange;

		TestOrder order(publisher, publisher, exchange);

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

				std::shared_ptr<const OrderSummaryMessage> summaryPtr = publisher.messageMap[sub];
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
	}

	TEST(OrderTest, SendThenCancelCase)
	{
		Dispatcher dispatcher(2);
		DummyPublisher publisher(dispatcher);
		DummyExchange exchange;

		TestOrder order(publisher, publisher, exchange);

		// send
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
			ASSERT_TRUE(exchange.statusMap[message.orderId] == OrderStatus::LIVE);

			ASSERT_TRUE(!publisher.messageMap.empty());

			{
				Subscription sub = {SourceType::ALL, DataType::ORDER_SUMMARY, message.orderId};
				ASSERT_TRUE(publisher.messageMap.find(sub) != publisher.messageMap.end());

				std::shared_ptr<const OrderSummaryMessage> summaryPtr = publisher.messageMap[sub];
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

		// cancel
		{
			std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
			OrderMessage& message = *ptr;

			message.orderId = 1;
			message.status = OrderStatus::CANCELLED;

			order.consume(ptr);

			ASSERT_TRUE(!exchange.statusMap.empty());
			ASSERT_TRUE(exchange.statusMap.find(message.orderId) != exchange.statusMap.end());
			ASSERT_TRUE(exchange.statusMap[message.orderId] == OrderStatus::CANCELLED);

			ASSERT_TRUE(!publisher.messageMap.empty());

			{
				Subscription sub = {SourceType::ALL, DataType::ORDER_SUMMARY, message.orderId};
				ASSERT_TRUE(publisher.messageMap.find(sub) != publisher.messageMap.end());

				std::shared_ptr<const OrderSummaryMessage> summaryPtr = publisher.messageMap[sub];
				ASSERT_TRUE(summaryPtr);

				const OrderSummaryMessage& summary = *summaryPtr;
				ASSERT_TRUE(summary.orderId == message.orderId);
				ASSERT_TRUE(summary.instrumentId == 2);
				ASSERT_TRUE(summary.price == 20000.0);
				ASSERT_TRUE(summary.totalQty == 1);
				ASSERT_TRUE(summary.side == Side::BID);
				ASSERT_TRUE(summary.status == OrderStatus::PENDING_CANCEL);
			}
		}
	}

}

