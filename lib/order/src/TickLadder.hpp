/*
 * TickLadder.hpp
 *
 *  Created on: Oct 6, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_TICKLADDER_HPP_
#define LIB_ORDER_SRC_TICKLADDER_HPP_

#include <cstdint>
#include <vector>

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
		}

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

		// The tick bands.
		std::vector<TickBand> ticks;
	};
}



#endif /* LIB_ORDER_SRC_TICKLADDER_HPP_ */
