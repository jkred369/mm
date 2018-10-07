/*
 * TickLadder.cpp
 *
 *  Created on: Oct 6, 2018
 *      Author: suoalex
 */

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include <NativeDefinition.hpp>
#include <MathUtil.hpp>

#include "TickLadder.hpp"

mm::Logger mm::TickLadder::logger;

namespace mm
{
	TickBand::TickBand(double lowerBound, double tickSize) : lowerBound(lowerBound), tickSize(tickSize), tickInverse(1 / tickSize)
	{
		if (lowerBound < 0)
		{
			throw std::invalid_argument("Lower bound cannot be smaller than 0.");
		}

		if (tickSize <= 0)
		{
			throw std::invalid_argument("Tick size must be positive.");
		}

		if (std::isnan(tickInverse))
		{
			throw std::invalid_argument("Invalid tick size as NaN or 0.");
		}
	}

	double TickLadder::roundToTick(double price) const
	{
		if (UNLIKELY(price < 0))
		{
			LOGERR("Cannot round negative price: {}", price);
			return 0.0;
		}

		std::size_t i = 0;
		for (; i < ticks.size(); ++i)
		{
			if (ticks[i].lowerBound > price)
			{
				break;
			}
		}

		const TickBand& tick = ticks[i - 1];
		return MathUtil::round(tick.lowerBound + std::lround((price - tick.lowerBound) * tick.tickInverse) * tick.tickSize);
	}

	double TickLadder::tickUp(double price) const
	{
		if (UNLIKELY(price < 0))
		{
			LOGERR("Cannot tick up negative price: {}", price);
			return 0.0;
		}

		std::size_t i = 0;
		for (; i < ticks.size(); ++i)
		{
			if (ticks[i].lowerBound > price)
			{
				break;
			}
		}

		return MathUtil::round(price + ticks[i - 1].tickSize);
	}

	double TickLadder::tickDown(double price) const
	{
		if (UNLIKELY(price < 0))
		{
			LOGERR("Cannot tick down negative price: {}", price);
			return 0.0;
		}

		if (UNLIKELY(price == 0.0))
		{
			return 0.0;
		}

		std::size_t i = 0;
		for (; i < ticks.size(); ++i)
		{
			if (ticks[i].lowerBound >= price)
			{
				break;
			}
		}

		const double result = MathUtil::round(price - ticks[i - 1].tickSize);
		return result >= 0 ? result : 0;
	}

	double TickLadder::tickMove(double price, std::int64_t tickCount) const
	{
		if (UNLIKELY(price < 0))
		{
			LOGERR("Cannot tick move negative price: {}", price);
			return 0.0;
		}

		std::size_t i = 0;
		for (; i < ticks.size(); ++i)
		{
			if (ticks[i].lowerBound > price)
			{
				break;
			}
		}

		// direction based start point
		i -= tickCount > 0 ? 1 : 0;

		double result = price;
		const std::int64_t direction = tickCount > 0 ? 1 : -1;

		for (std::int64_t count = std::abs(tickCount); count != 0; )
		{
			const double upperBound = i < ticks.size() - 1 ? ticks[i + 1].lowerBound : std::numeric_limits<double>::max();
			const long maxCount = std::lround((upperBound - ticks[i].lowerBound) * ticks[i].tickInverse);
			const long actualCount = std::min(count, maxCount);

			result += actualCount * ticks[i].tickSize * direction;
			count -= actualCount;

			if (direction < 0 && result <= ticks[i].lowerBound)
			{
				--i;
			}
			else if (direction > 0 && result >= ticks[i + 1].lowerBound)
			{
				++i;
			}
		}

		return result <= 0 ? 0.0 : MathUtil::round(result);
	}
}


