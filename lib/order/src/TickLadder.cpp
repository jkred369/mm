/*
 * TickLadder.cpp
 *
 *  Created on: Oct 6, 2018
 *      Author: suoalex
 */

#include <cmath>
#include <stdexcept>

#include "TickLadder.hpp"

namespace mm
{
	TickBand::TickBand(double lowerBound, double tickSize) : lowerBound(lowerBound), tickSize(tickSize), tickInverse(1 / tickSize)
	{
		if (std::isnan(tickInverse))
		{
			throw std::invalid_argument("Invalid tick size as NaN or 0.");
		}
	}

	double TickLadder::tickUp(double price) const
	{
		return 0;
	}

	double TickLadder::tickDown(double price) const
	{
		return 0;
	}

	double TickLadder::tickMove(double price, std::int64_t tickCount) const
	{
		return 0;
	}
}


