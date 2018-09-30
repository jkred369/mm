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
			side(Side::BID),
			totalQty(0),
			tradedQty(0),
			price(0.0),
			avgTradedPrice(0.0),
			status(OrderStatus::NEW)
	{
	}

	bool OrderSummaryMessage::equals(const OrderSummaryMessage& rhs) const
	{
		return orderId == rhs.orderId &&
				instrumentId == rhs.instrumentId &&
				side == rhs.side &&
				totalQty == rhs.totalQty &&
				tradedQty == rhs.tradedQty &&
				price == rhs.price &&
				avgTradedPrice == rhs.avgTradedPrice &&
				status == rhs.status;
	}

}


