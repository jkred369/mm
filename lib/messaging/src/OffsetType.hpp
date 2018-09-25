/*
 * OffsetType.hpp
 *
 *  Created on: Sep 25, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_OFFSETTYPE_HPP_
#define LIB_MESSAGING_SRC_OFFSETTYPE_HPP_

namespace mm
{
	//
	// This enum denotes the "offset type" requred from some exchange.
	//
	//
	//
	enum class OffsetType : std::int64_t
	{
		// this order opens a position.
		OPEN = 0,

		// this order closes a position.
		CLOSE = 1,

		// this order force close a position.
		FORCE_CLOSE = 2,

		// this order close a position till end of today
		CLOSE_TODAY = 3,

		// this order close a position from yesterday
		CLOSE_T_MINUS_1 = 4,
	};
}



#endif /* LIB_MESSAGING_SRC_OFFSETTYPE_HPP_ */
