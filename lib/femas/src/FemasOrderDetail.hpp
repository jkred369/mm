/*
 * FemasOrderDetail.hpp
 *
 *  Created on: Sep 25, 2018
 *      Author: suoalex
 */

#ifndef LIB_FEMAS_SRC_FEMASORDERDETAIL_HPP_
#define LIB_FEMAS_SRC_FEMASORDERDETAIL_HPP_

#include <cstring>

#include <femas/USTPFtdcUserApiDataType.h>

namespace mm
{
	//
	// This class holds all the order specific detail for the order session to fill in session specific data.
	//
	struct FemasOrderDetail
	{
		//
		// Default constructor.
		//
		FemasOrderDetail() : hedgeFlag(USTP_FTDC_CHF_Speculation), isAutoSuspend(0), exchangeId("")
		{
		}

		//
		// Constructor.
		//
		// hedgeFlag : The hedge flag needed.
		// isAutoSuspend : Flag if auto suspend is needed.
		// exchangeId : The exchange to send order to.
		//
		FemasOrderDetail(
				char hedgeFlag,
				int isAutoSuspend,
				const std::string exchangeId) :
					hedgeFlag(hedgeFlag),
					isAutoSuspend(isAutoSuspend),
					exchangeId(exchangeId)
		{
			if (exchangeId.size() + 1 > sizeof(TUstpFtdcExchangeIDType))
			{
				throw std::invalid_argument("Exchange ID can be at most 11 chars.");
			}
		}

		// the hedge flag
		const TUstpFtdcHedgeFlagType hedgeFlag;

		// flag if order should be auto suspend
		const TUstpFtdcBoolType isAutoSuspend;

		// the exchange ID
		const std::string exchangeId;
	};
}



#endif /* LIB_FEMAS_SRC_FEMASORDERDETAIL_HPP_ */
