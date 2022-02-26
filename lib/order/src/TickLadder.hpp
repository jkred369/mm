/*
 * TickLadder.hpp
 *
 *  Created on: Oct 6, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_TICKLADDER_HPP_
#define LIB_ORDER_SRC_TICKLADDER_HPP_

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include <Logger.hpp>

namespace mm
{
	//
	// The definition of a tick band.
	//
	struct TickBandData
	{
		//
		// Constructor.
		//
		// lowerBound : The lower bound for the tick band.
		// tickSize : The tick size.
		//
		TickBandData(double lowerBound, double tickSize);

		// The lower bound of the tick band.
		double lowerBound;

		// The tick size.
		double tickSize;

		// The inverse tick for performance.
		double tickInverse;
	};

	//
	// This class implements a tick ladder concept.
	//
	class TickLadder
	{
	public:

		//
		// An enhanced tick band for performance.
		//
		struct TickBand : TickBandData
		{
			TickBand(TickBandData& tickBandData) : TickBandData(tickBandData)
			{
			}

			// index of this band in the whole tick band
			std::int64_t index {0};

			// upper bound for this band
			double upperBound {0.0};

			// tick count in this band
			std::int64_t maxTickCount {0};
		};

		//
		// Container style constructor.
		//
		// begin : The begin iterator for tick band.
		// end : The end iterator for tick band.
		//
		template<typename Iterator> TickLadder(Iterator begin, Iterator end)
		{
			for (Iterator i = begin; i < end; ++i)
			{
				ticks.emplace_back(*i);
			}

			if (ticks.empty())
			{
				throw std::invalid_argument("Empty tick band is not allowed.");
			}

			std::sort(ticks.begin(), ticks.end(), [] (const TickBand& lhs, const TickBand& rhs) {
				return lhs.lowerBound < rhs.lowerBound;
			});

			if (ticks[0].lowerBound != 0.0)
			{
				throw std::invalid_argument("Lowest tick band must have lower bound = 0.");
			}

			for (std::int64_t i = 0; i < (std::int64_t) ticks.size(); ++i)
			{
				TickBand& band = ticks[i];
				band.index = i;

				if (i + 1 < (std::int64_t) ticks.size())
				{
					band.upperBound = ticks[i + 1].lowerBound;
				}
				else
				{
					band.upperBound = std::numeric_limits<double>::max();
				}

				band.maxTickCount = std::lround((band.upperBound - band.lowerBound) * band.tickInverse);
			}
		}

		//
		// Round the price to nearest tick.
		//
		// price : The price to round.
		// hint : Tick band hint, if any.
		//
		// return : The nearest tick price from price.
		//
		double roundToTick(double price, const TickBand* hint = nullptr) const;

		//
		// Tick up the given price.
		//
		// price : The original price.
		// hint : Tick band hint, if any.
		//
		// return : The price after tick up.
		//
		double tickUp(double price, const TickBand* hint = nullptr) const;

		//
		// Tick down the given price.
		//
		// price : The original price.
		// hint : Tick band hint, if any.
		//
		// return : The price after tick down; or 0 as the lower threshold.
		//
		double tickDown(double price, const TickBand* hint = nullptr) const;

		//
		// Move the tick by a given count.
		//
		// price : The original price.
		// tickCount : The number of ticks to move. Negative for moving down.
		// hint : Tick band hint, if any.
		//
		// return : The price after moving.
		//
		inline double tickMove(double price, std::int64_t tickCount, const TickBand* hint = nullptr) const
		{
			return tickCount > 0 ? tickMoveUp(price, tickCount) : tickMoveDown(price, std::abs(tickCount));
		}

		//
		// Move the tick up by a given count.
		//
		// price : The original price.
		// tickCount : The number of ticks to move up.
		// hint : Tick band hint, if any.
		//
		// return : The price after moving.
		//
		double tickMoveUp(double price, std::int64_t tickCount, const TickBand* hint = nullptr) const;

		//
		// Move the tick down by a given count.
		//
		// price : The original price.
		// tickCount : The number of ticks to move down.
		// hint : Tick band hint, if any.
		//
		// return : The price after moving.
		//
		double tickMoveDown(double price, std::int64_t tickCount, const TickBand* hint = nullptr) const;

		//
		// Get the tick band so the for the band, lower bound <= price < upper bound.
		//
		// price : The price.
		// hint : A hint of the band.
		//
		// return : The band reference for this price.
		//
		inline const TickBand* getBand(double price, const TickBand* hint = nullptr) const
		{
			if (hint && hint->lowerBound <= price && hint->upperBound > price)
			{
				return hint;
			}

			for (const TickBand& tick : ticks)
			{
				if (tick.lowerBound <= price && tick.upperBound > price)
				{
					return &tick;
				}
			}

			return nullptr;
		}

	private:

		// Logger of the class.
		static Logger logger;

		// The tick bands.
		std::vector<TickBand> ticks;
	};
}



#endif /* LIB_ORDER_SRC_TICKLADDER_HPP_ */
