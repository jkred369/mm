/*
 * TickLadder.cpp
 *
 *  Created on: Oct 6, 2018
 *      Author: suoalex
 */

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <NativeDefinition.hpp>
#include <MathUtil.hpp>

#include "TickLadder.hpp"

mm::Logger mm::TickLadder::logger;

namespace mm
{
	TickBandData::TickBandData(double lowerBound, double tickSize) : lowerBound(lowerBound), tickSize(tickSize), tickInverse(1 / tickSize)
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

	double TickLadder::roundToTick(double price, const TickBand* hint) const
	{
		if (UNLIKELY(!MathUtil::validPrice(price)))
		{
			return 0.0;
		}

		const TickBand* tick = getBand(price, hint);
		if (UNLIKELY(!tick))
		{
			LOGFATAL("Cannot get tick band for price {}", price);
			return NAN;
		}

		return MathUtil::round(tick->lowerBound +
				std::lround((price - tick->lowerBound) * tick->tickInverse) * tick->tickSize);
	}

	double TickLadder::tickUp(double price, const TickBand* hint) const
	{
		if (UNLIKELY(price < 0.0))
		{
			LOGERR("Cannot tick up negative price: {}", price);
			return 0.0;
		}

		const TickBand* tick = getBand(price, hint);
		if (UNLIKELY(!tick))
		{
			LOGFATAL("Cannot get tick band for price {}", price);
			return NAN;
		}

		return MathUtil::round(price + tick->tickSize);
	}

	double TickLadder::tickDown(double price, const TickBand* hint) const
	{
		if (UNLIKELY(!MathUtil::validPrice(price)))
		{
			LOGERR("Cannot tick down non-positive price: {}", price);
			return 0.0;
		}

		const TickBand* tick = getBand(price, hint);
		if (UNLIKELY(!tick))
		{
			LOGFATAL("Cannot get tick band for price {}", price);
			return NAN;
		}

		if (MathUtil::equals(tick->lowerBound, price) && tick->index)
		{
			tick = &ticks[tick->index - 1];
		}

		const double result = MathUtil::round(price - tick->tickSize);
		return MathUtil::validPrice(result) ? result : 0.0;
	}

	double TickLadder::tickMoveUp(double price, std::int64_t tickCount, const TickBand* hint) const
	{
		if (UNLIKELY(price < 0.0))
		{
			LOGERR("Cannot tick move negative price: {}", price);
			return 0.0;
		}

		if (UNLIKELY(!tickCount))
		{
			return price;
		}

		if (UNLIKELY(tickCount < 0))
		{
			return tickMoveDown(price, std::abs(tickCount), hint);
		}

		const TickBand* tick = getBand(price, hint);
		if (UNLIKELY(!tick))
		{
			LOGFATAL("Cannot get tick band for price {}", price);
			return NAN;
		}

		double result = price;

		for (size_t count = tickCount; count != 0; )
		{
			if (MathUtil::less_than(result + tick->tickSize * count, tick->upperBound))
			{
				result += tick->tickSize * count;
				break;
			}

			count -= std::lround((tick->upperBound - result) * tick->tickInverse);
			result = tick->upperBound;

			if ((size_t) tick->index + 1 < ticks.size())
			{
				tick = &ticks[tick->index + 1];
			}
			else
			{
				LOGERR("Exhausted tick ladder for tick move up {}, {}", price, tickCount);
				break;
			}
		}

		return MathUtil::validPrice(result) ? 0.0 : MathUtil::round(result);
	}

	double TickLadder::tickMoveDown(double price, std::int64_t tickCount, const TickBand* hint) const
	{
		if (UNLIKELY(price < 0.0))
		{
			LOGERR("Cannot tick move negative price: {}", price);
			return 0.0;
		}

		if (UNLIKELY(!tickCount))
		{
			return price;
		}

		if (UNLIKELY(tickCount < 0))
		{
			return tickMoveUp(price, std::abs(tickCount), hint);
		}

		const TickBand* tick = getBand(price, hint);
		if (UNLIKELY(!tick))
		{
			LOGFATAL("Cannot get tick band for price {}", price);
			return NAN;
		}

		if (MathUtil::equals(tick->lowerBound, price) && tick->index)
		{
			tick = &ticks[tick->index - 1];
		}

		double result = price;

		for (size_t count = tickCount; count != 0; )
		{
			if (MathUtil::greater_than(result - tick->tickSize * count, tick->upperBound))
			{
				result -= tick->tickSize * count;
				break;
			}

			count -= std::lround((result - tick->lowerBound) * tick->tickInverse);
			result = tick->lowerBound;

			if (tick->index > 0)
			{
				tick = &ticks[tick->index - 1];
			}
			else
			{
				LOGERR("Exhausted tick ladder for tick move up {}, {}", price, tickCount);
				break;
			}
		}

		return MathUtil::validPrice(result) ? 0.0 : MathUtil::round(result);
	}
}


