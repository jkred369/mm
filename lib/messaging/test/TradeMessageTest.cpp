/*
 * TradeMessageTest.cpp
 *
 *  Created on: Oct 26, 2018
 *      Author: suoalex
 */

#include <gtest/gtest.h>

#include "StringBuffer.hpp"
#include "TradeMessage.hpp"

namespace mm
{
	TEST(TradeMessageTest, SerializationCase)
	{
		TradeMessage message;

		message.instrumentId = 1;
		message.orderId = 2;
		message.executionId = "12345";
		message.side = Side::BID;
		message.price = 2.0;
		message.qty = 1000;
		message.strategyId = 5;
		message.timestamp = DateTimeUtil::now();

		StringBuffer buffer;
		message.serialize(buffer);

		TradeMessage message2;
		message2.deserialize(buffer);

		ASSERT_TRUE(message2.instrumentId == 1);
		ASSERT_TRUE(message2.orderId == 2);
		ASSERT_TRUE(message2.executionId == "12345");
		ASSERT_TRUE(message2.side == Side::BID);
		ASSERT_TRUE(message2.price == 2.0);
		ASSERT_TRUE(message2.qty == 1000);
		ASSERT_TRUE(message2.strategyId == 5);

		ASSERT_TRUE(message == message2);
	}
}


