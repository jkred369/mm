/*
 * TradeMessageTest.cpp
 *
 *  Created on: Oct 26, 2018
 *      Author: suoalex
 */

#include <gtest/gtest.h>

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
	}
}


