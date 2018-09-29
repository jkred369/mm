/*
 * OrderMessage.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include "OrderMessage.hpp"

namespace mm
{
	OrderMessage::OrderMessage() :
		orderId(0),
		instrumentId(0),
		side(Side::BID),
		totalQty(0),
		price(0.0),
		status(OrderStatus::NEW),
		type(OrderType::IOC),
		offsetType(OffsetType::OPEN)
	{
	}

}


