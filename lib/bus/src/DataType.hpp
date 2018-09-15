/*
 * DataType.hpp
 *
 *  Created on: Mar 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_DATATYPE_HPP_
#define LIB_BUS_SRC_DATATYPE_HPP_

#include <cstdint>

namespace mm
{
	//
	// The enum class for data type
	//
	enum class DataType : std::int16_t
	{
	    // Subscription request.
	    SUB_REQ = 0,

   	    // System heart beat.
   	    HEART_BEAT = 1,

   	    // System heart beat reply.
   	    HEART_BEAT_REPLY = 2,

	    // Market spot.
	    SPOT = 3,

		// full market data (level 2)
		MARKET_DATA = 4,

	    // FX.
	    FX = 5,

	    // Product definition.
	    PRODUCT = 6,

	    // Dividend schedule.
	    DIVIDEND = 7,

	    // Market calendar.
	    CALENDAR = 8,

	    // Yield curve.
	    YIELD_CURVE = 9,

	    // Computed Spot.
	    COMPUTED_SPOT = 10,

		// Computed market data (level 2)
		COMPUTED_MARKET_DATA = 11,

	    // Computed basket weight.
	    BASKET_WEIGHT = 12,

	    // The basket info.
	    BASKET_INFO = 13,

	    // The basket wave info.
	    BASKET_WAVE = 14,

	    // Bps information.
	    BPS = 15,

	    // Algorithm command.
	    ALGO_COMMAND = 16,

	    // New order single event.
	    NEW_ORDER = 17,

		// New order rejection event.
		NEW_REJECT = 18,

	    // Order execution report event.
	    EXEC_REPORT = 19,

	    // Order replacement request.
	    REPLACE = 20,

	    // Order cancel request.
	    CANCEL = 21,

	    // Order replace/cancel reject.
	    CANCEL_REJECT = 22,

	    // Summary of order current status.
	    ORDER_SUMMARY = 23,

	};
}



#endif /* LIB_BUS_SRC_DATATYPE_HPP_ */
