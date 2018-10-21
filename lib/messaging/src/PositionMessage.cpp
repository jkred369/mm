/*
 * PositionMessage.cpp
 *
 *  Created on: Oct 21, 2018
 *      Author: suoalex
 */

#include "PositionMessage.hpp"

namespace mm
{
	PositionMessage::PositionMessage() :
			instrumentId(0),
			totalQty(0),
			sodQty(0),
			dailyBoughtQty(0),
			dailySoldQty(0)
	{
	}

	bool PositionMessage::equals(const PositionMessage& rhs) const
	{
		return instrumentId == rhs.instrumentId &&
				totalQty == rhs.totalQty &&
				sodQty == rhs.sodQty &&
				dailyBoughtQty == rhs.dailyBoughtQty &&
				dailySoldQty == rhs.dailySoldQty;
	}

}
