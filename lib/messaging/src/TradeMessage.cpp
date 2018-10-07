/*
 * ExecutionMessage.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include "TradeMessage.hpp"

namespace mm
{
	TradeMessage::TradeMessage() : orderId(0), instrumentId(0), strategyId(0), executionId(0), side(Side::BID), qty(0), price(0.0)
	{
	}

	bool TradeMessage::equals(const TradeMessage& rhs) const
	{
		return orderId == rhs.orderId &&
				instrumentId == rhs.instrumentId &&
				strategyId == rhs.strategyId &&
				executionId == rhs.executionId &&
				side == rhs.side &&
				qty == rhs.qty &&
				price == rhs.price;
	}

}


