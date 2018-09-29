/*
 * ExecutionMessage.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include "ExecutionMessage.hpp"

namespace mm
{
	ExecutionMessage::ExecutionMessage() : orderId(0), instrumentId(0), executionId(0), side(Side::BID), qty(0), price(0.0), status(OrderStatus::NEW)
	{
	}

	bool ExecutionMessage::equals(const ExecutionMessage& rhs) const
	{
		return orderId == rhs.orderId &&
				instrumentId == rhs.instrumentId &&
				executionId == rhs.executionId &&
				side == rhs.side &&
				qty = rhs.qty &&
				price == rhs.price &&
				status == rhs.status;
	}

}


