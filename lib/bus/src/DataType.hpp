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
		// All data type
		ALL = 0,

	    // Subscription request.
	    SUB_REQ = 1,

   	    // System heart beat.
   	    HEART_BEAT = 2,

   	    // System heart beat reply.
   	    HEART_BEAT_REPLY = 3,

	    // Market spot.
	    SPOT = 4,

		// full market data (level 2)
		MARKET_DATA = 5,

	    // FX.
	    FX = 6,

	    // Product definition.
	    PRODUCT = 7,

	    // Dividend schedule.
	    DIVIDEND = 8,

	    // Market calendar.
	    CALENDAR = 9,

	    // Yield curve.
	    YIELD_CURVE = 10,

	    // Computed Spot.
	    COMPUTED_SPOT = 11,

		// Computed market data (level 2)
		COMPUTED_MARKET_DATA = 12,

	    // Computed basket weight.
	    BASKET_WEIGHT = 13,

	    // The basket info.
	    BASKET_INFO = 14,

	    // The basket wave info.
	    BASKET_WAVE = 15,

	    // Bps information.
	    BPS = 16,

	    // Algorithm command.
	    ALGO_COMMAND = 17,

	    // New order single event.
	    NEW_ORDER = 18,

		// New order rejection event.
		NEW_REJECT = 19,

	    // Order execution report event.
	    EXEC_REPORT = 20,

	    // Order replacement request.
	    REPLACE = 21,

	    // Order cancel request.
	    CANCEL = 22,

	    // Order replace/cancel reject.
	    CANCEL_REJECT = 23,

	    // Summary of order current status.
	    ORDER_SUMMARY = 24,

	};
}



#endif /* LIB_BUS_SRC_DATATYPE_HPP_ */
