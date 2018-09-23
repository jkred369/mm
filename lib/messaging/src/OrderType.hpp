/*
 * OrderType.hpp
 *
 *  Created on: Sep 23, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDERTYPE_HPP_
#define LIB_ORDER_SRC_ORDERTYPE_HPP_

#include <cstdint>

namespace mm
{
	//
	// The order type enum.
	//
	enum class OrderType : std::int64_t
	{
		// limit order
		LIMIT = 0,

		// market order
		MARKET = 1,

		// IOC order (immediate or cancel)
		IOC = 2,

		// FOK order (fill or kill)
		FOK = 3
	};

}

#endif /* LIB_ORDER_SRC_ORDERTYPE_HPP_ */
