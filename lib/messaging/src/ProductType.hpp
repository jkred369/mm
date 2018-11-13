/*
 * ProductType.hpp
 *
 *  Created on: Sep 27, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_PRODUCTTYPE_HPP_
#define LIB_MESSAGING_SRC_PRODUCTTYPE_HPP_

#include <cstdint>

namespace mm
{
	//
	// Defines the product type.
	//
	enum class ProductType : std::int64_t
	{
		// Stock.
		STOCK = 0,

		// Equity index.
		INDEX = 1,

		// FX rate.
		FX = 2,

		// Commodity (cash).
		COMMODITY = 3,

		// ETF.
		ETF = 4,

		// Future (of any underlying).
		FUTURE = 5,

		// European option.
		EUROPEAN = 6,

		// American option.
		AMERICAN = 7,

		// Government bond
		GVMT_BOND = 8,
	};
}



#endif /* LIB_MESSAGING_SRC_PRODUCTTYPE_HPP_ */
