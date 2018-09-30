/*
 * FemasMarketDataSessionTest.cpp
 *
 *  Created on: Sep 30, 2018
 *      Author: suoalex
 */

#include <gtest/gtest.h>

#include "FemasMarketDataSession.hpp"

namespace mm
{
	TEST(FemasMarketDataSessionTest, ConnectionCase)
	{
		FemasUserDetail detail;
		detail.brokerId = "0110";
		detail.frontAddress = "tcp://101.251.67.3:17101";
	}

}





