/*
 * MarketDataMessage.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include "MarketDataMessage.hpp"

namespace mm
{
	MarketDataLevel::MarketDataLevel() : price(0.0), qty(0)
	{
	}

	MarketDataMessage::MarketDataMessage() :
			instrumentId(0),
			open(0.0),
			close(0.0),
			high(0.0),
			low(0.0),
			last(0.0),
			highLimit(0.0),
			lowLimit(0.0),
			volume(0),
			turnover(0.0)
	{
	}

}
