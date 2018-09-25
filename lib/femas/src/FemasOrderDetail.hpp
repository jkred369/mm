/*
 * FemasOrderDetail.hpp
 *
 *  Created on: Sep 25, 2018
 *      Author: suoalex
 */

#ifndef LIB_FEMAS_SRC_FEMASORDERDETAIL_HPP_
#define LIB_FEMAS_SRC_FEMASORDERDETAIL_HPP_

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
		FemasOrderDetail() : hedgeFlag(USTP_FTDC_CHF_Speculation), isAutoSuspend(0)
		{
		}

		//
		// Constructor.
		//
		// hedgeFlag : The hedge flag needed.
		// isAutoSuspend : Flag if auto suspend is needed.
		//
		FemasOrderDetail(char hedgeFlag, int isAutoSuspend) : hedgeFlag(hedgeFlag), isAutoSuspend(isAutoSuspend)
		{
		}

		// the hedge flag
		const TUstpFtdcHedgeFlagType hedgeFlag;

		// flag if order should be auto suspend
		const TUstpFtdcBoolType isAutoSuspend;
	};
}



#endif /* LIB_FEMAS_SRC_FEMASORDERDETAIL_HPP_ */
