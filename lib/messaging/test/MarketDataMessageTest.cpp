/*
 * MarketDataMessageTest.cpp
 *
 *  Created on: Oct 2, 2018
 *      Author: suoalex
 */

#include <iostream>

#include <gtest/gtest.h>

#include <Timer.hpp>

#include "MarketDataMessage.hpp"

namespace mm
{
	TEST(MarketDataMessageTest, CopyPerformanceCase)
	{
		MarketDataMessage md1;
		MarketDataMessage md2;

		md1.close = 100;
		md1.high = 110;
		md1.highLimit = 110;
		md1.instrumentId = 4;
		md1.last = 104;
		md1.low = 100;
		md1.lowLimit = 100;
		md1.open = 102;
		md1.turnover = 200000;
		md1.volume = 4000;

		for (std::size_t i = 0; i < 2; ++i)
		{
			for (std::size_t j = 0; j < MarketDataMessage::MAX_DEPTH; ++j)
			{
				md1.levels[i][j].price = 100.0;
				md1.levels[i][j].qty = 10000;
			}
		}

		const size_t count = 10000;
		HighResTimer timer;

		const double start = timer.getTimeInNanos();
		for (std::size_t i = 0; i < count; ++i)
		{
			md2 = md1;
			md1.turnover += 10000;
			md1.volume += 100;
		}

		const double end = timer.getTimeInNanos();
		std::cout << "Market data copy avg (ns): " << (end - start) / count << std::endl;
	}
}


