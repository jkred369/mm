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
			std::vector<TickBandData> ticks = {};
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
		std::vector<TickBandData> ticks = { {0, 0.001}, {0.1, 0.005}, {0.25, 0.01} };
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

	TEST(TickLadderTest, TickUpCase)
	{
		std::vector<TickBandData> ticks = { {0, 0.001}, {0.1, 0.005}, {0.25, 0.01} };
		TickLadder ladder(ticks.begin(), ticks.end());

		ASSERT_TRUE(MathUtil::equals(ladder.tickUp(0.0), 0.001));
		ASSERT_TRUE(MathUtil::equals(ladder.tickUp(0.099), 0.1));
		ASSERT_TRUE(MathUtil::equals(ladder.tickUp(0.245), 0.25));

		ASSERT_TRUE(MathUtil::equals(ladder.tickUp(0.1), 0.105));
		ASSERT_TRUE(MathUtil::equals(ladder.tickUp(0.25), 0.26));

		ASSERT_TRUE(MathUtil::equals(ladder.tickUp(0.003), 0.004));
		ASSERT_TRUE(MathUtil::equals(ladder.tickUp(0.27), 0.28));
		ASSERT_TRUE(MathUtil::equals(ladder.tickUp(0.105), 0.11));
	}

	TEST(TickLadderTest, TickDownCase)
	{
		std::vector<TickBandData> ticks = { {0, 0.001}, {0.1, 0.005}, {0.25, 0.01} };
		TickLadder ladder(ticks.begin(), ticks.end());

		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.0), 0.0));
		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.1), 0.099));
		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.25), 0.245));

		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.001), 0.0));
		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.105), 0.1));
		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.26), 0.25));

		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.003), 0.002));
		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.27), 0.26));
		ASSERT_TRUE(MathUtil::equals(ladder.tickDown(0.11), 0.105));
	}

	TEST(TickLadderTest, TickMoveCase)
	{
		std::vector<TickBandData> ticks = { {0, 0.001}, {0.1, 0.005}, {0.25, 0.01} };
		TickLadder ladder(ticks.begin(), ticks.end());

		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.0, 0), 0.0));
		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.0, 1), 0.001));
		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.0, -1), 0.0));

		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.1, 0), 0.1));
		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.1, 1), 0.105));
		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.1, -1), 0.099));

//		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.25, 0), 0.25));
//		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.25, 1), 0.26));
//		ASSERT_TRUE(MathUtil::equals(ladder.tickMove(0.25, -1), 0.245));
	}

}


