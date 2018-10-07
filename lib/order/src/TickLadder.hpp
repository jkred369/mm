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
#include <stdexcept>
#include <vector>

#include <Logger.hpp>

namespace mm
{
	//
	// The definition of a tick band.
	//
	struct TickBand
	{
		//
		// Constructor.
		//
		// lowerBound : The lower bound for the tick band.
		// tickSize : The tick size.
		//
		TickBand(double lowerBound, double tickSize);

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
		// Container style constructor.
		//
		// begin : The begin iterator for tick band.
		// end : The end iterator for tick band.
		//
		template<typename Iterator> TickLadder(Iterator begin, Iterator end)
		{
			for (Iterator i = begin; i < end; ++i)
			{
				ticks.push_back(*i);
			}

			if (ticks.empty())
			{
				throw std::invalid_argument("Empty tick band is not allowed.");
			}

			std::sort(ticks.begin(), ticks.end(), [] (const TickBand& lhs, const TickBand& rhs) {
				return lhs.lowerBound < rhs.lowerBound;
			});

			if (ticks[0].lowerBound != 0)
			{
				throw std::invalid_argument("Lowest tick band must have lower bound = 0.");
			}
		}

		//
		// Round the price to nearest tick.
		//
		// price : The price to round.
		//
		// return : The nearest tick price from price.
		//
		double roundToTick(double price) const;

		//
		// Tick up the given price.
		//
		// price : The original price.
		//
		// return : The price after tick up.
		//
		double tickUp(double price) const;

		//
		// Tick down the given price.
		//
		// price : The original price.
		//
		// return : The price after tick down; or 0 as the lower threshold.
		//
		double tickDown(double price) const;

		//
		// Move the tick by a given count.
		//
		// price : The original price.
		// tickCount : The number of ticks to move. Negative for moving down.
		//
		// return : The price after moving.
		//
		double tickMove(double price, std::int64_t tickCount) const;

	private:

		// Logger of the class.
		static Logger logger;

		// The tick bands.
		std::vector<TickBand> ticks;
	};
}



#endif /* LIB_ORDER_SRC_TICKLADDER_HPP_ */
