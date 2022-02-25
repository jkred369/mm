/*
 * TickLadderTest.cpp
 *
 *  Created on: Feb 25, 2022
 *      Author: alexsuo
 */

#include <iostream>
#include <vector>

#include <gtest/gtest.h>

#include <MathUtil.hpp>
#include <TickLadder.hpp>

namespace mm
{
	TEST(TickLadderTest, DummyCase)
	{
		bool caught = false;

		try
		{
			std::vector<TickBand> ticks = {};
			TickLadder ladder(ticks.begin(), ticks.end());
		}
		catch (std::invalid_argument& e)
		{
			caught = true;
		}

		ASSERT_TRUE(caught);
	}

	TEST(TickLadderTest, RoundCase)
	{
		std::vector<TickBand> ticks = { {0, 0.001}, {0.1, 0.005}, {0.25, 0.01} };
		TickLadder ladder(ticks.begin(), ticks.end());

		ASSERT_TRUE(ladder.roundToTick(0.023999999) >= 0.024);

		ASSERT_TRUE(MathUtil::equals(ladder.roundToTick(0.0), 0.0));
		ASSERT_TRUE(MathUtil::equals(ladder.roundToTick(0.0005), 0.001));
		ASSERT_TRUE(MathUtil::equals(ladder.roundToTick(0.0004), 0.0));

		ASSERT_TRUE(MathUtil::equals(ladder.roundToTick(0.023999999), 0.024));
		ASSERT_TRUE(MathUtil::equals(ladder.roundToTick(0.024000001), 0.024));

		ASSERT_TRUE(MathUtil::equals(ladder.roundToTick(0.25), 0.25));
		ASSERT_TRUE(MathUtil::equals(ladder.roundToTick(0.275), 0.28));
		ASSERT_TRUE(MathUtil::equals(ladder.roundToTick(0.2749), 0.27));
	}
}


