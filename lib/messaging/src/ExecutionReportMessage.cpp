/*
 * ExecutionReportMessage.cpp
 *
 *  Created on: Oct 6, 2018
 *      Author: suoalex
 */

#include "ExecutionReportMessage.hpp"

namespace mm
{
	ExecutionReportMessage::ExecutionReportMessage() :
			orderId(0),
			instrumentId(0),
			openQty(0),
			tradedQty(0),
			execQty(0),
			price(0.0),
			execPrice(0.0),
			status(OrderStatus::NEW),
			side(Side::BID)
	{
	}

	bool ExecutionReportMessage::equals(const ExecutionReportMessage& rhs) const
	{
		return orderId == rhs.orderId &&
				instrumentId == rhs.instrumentId &&
				openQty == rhs.openQty &&
				tradedQty == rhs.tradedQty &&
				execQty == rhs.execQty &&
				price == rhs.price &&
				execPrice == rhs.execPrice &&
				status == rhs.status &&
				side == rhs.side &&
				tradeTimestamp == rhs.tradeTimestamp &&
				executionId == rhs.executionId;
	}
}
