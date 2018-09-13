/*
 * FemasMarketDataSession.cpp
 *
 *  Created on: Sep 13, 2018
 *      Author: suoalex
 */

#include <FemasMarketDataSession.hpp>

namespace mm
{
	FemasMarketDataSession::FemasMarketDataSession() :
		session(CUstpFtdcMduserApi::CreateFtdcMduserApi())
	{
	}

	FemasMarketDataSession::~FemasMarketDataSession()
	{
	}
}
