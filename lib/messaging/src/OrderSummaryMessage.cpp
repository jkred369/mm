/*
 * OrderSummaryMessage.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include "OrderSummaryMessage.hpp"

namespace mm
{
	OrderSummaryMessage::OrderSummaryMessage() :
			orderId(0),
			instrumentId(0),
			strategyId(0),
			side(Side::BID),
			totalQty(0),
			tradedQty(0),
			price(0.0),
			tradedNotional(0.0),
			status(OrderStatus::NEW)
	{
	}

	bool OrderSummaryMessage::equals(const OrderSummaryMessage& rhs) const
	{
		return orderId == rhs.orderId &&
				instrumentId == rhs.instrumentId &&
				strategyId == rhs.strategyId &&
				side == rhs.side &&
				totalQty == rhs.totalQty &&
				tradedQty == rhs.tradedQty &&
				price == rhs.price &&
				tradedNotional == rhs.tradedNotional &&
				status == rhs.status;
	}

}


