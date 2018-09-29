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

	bool MarketDataLevel::equals(const MarketDataLevel& rhs) const
	{
		return price == rhs.price && qty == rhs.qty;
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

	bool MarketDataMessage::equals(const MarketDataMessage& rhs) const
	{
		if (instrumentId != rhs.instrumentId ||
				open != rhs.open ||
				close != rhs.close ||
				high != rhs.high ||
				low != rhs.low ||
				last != rhs.last ||
				highLimit != rhs.highLimit ||
				lowLimit != rhs.lowLimit ||
				volume != rhs.volume ||
				turnover != rhs.turnover)
		{
			return false;
		}

		for (std::size_t i = 0; i < 2; ++i)
		{
			for (std::size_t j = 0; j < MAX_DEPTH; ++j)
			{
				if (levels[i][j] != rhs.levels[i][j])
				{
					return false;
				}
			}
		}

		return true;
	}

}
