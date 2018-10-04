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
	struct DummyPublisher : PublisherAdapter<OrderSummaryMessage>
	{
		virtual const KeyType getKey() const override
		{
			return DispatchKey::ORDER;
		}

		virtual void publish(const Subscription& subscription, const std::shared_ptr<const OrderSummaryMessage>& message) override
		{
			messageMap[subscription] = message;
			PublisherAdapter<OrderSummaryMessage>::publish(subscription, message);
		}

		std::unordered_map<Subscription, std::shared_ptr<const OrderSummaryMessage> > messageMap;
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

	typedef Order<DummyExchange, NullObjectPool<OrderSummaryMessage> > TestOrder;

	TEST(OrderTest, SendCase)
	{
		DummyPublisher publisher;
		DummyExchange exchange;

		TestOrder order(publisher, exchange);

		{
			std::shared_ptr<OrderMessage> ptr = std::make_shared<OrderMessage> ();
			OrderMessage message = *ptr;

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
		}

	}

}

