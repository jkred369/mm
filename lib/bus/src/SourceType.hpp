/*
 * SourceType.hpp
 *
 *  Created on: Mar 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_SOURCETYPE_HPP_
#define LIB_BUS_SRC_SOURCETYPE_HPP_

#include <cstdint>

namespace mm
{
	//
	// The source type enum.
	//
	enum class SourceType : std::int16_t
	{
		// all source type
		ALL = 0,

		// femas market data source
		FEMAS_MARKET_DATA = 1,

		// femas order feedback
		FEMAS_ORDER = 2,

		// arb algos
		ARB = 3,

		// product service
		PRODUCT_SERVICE = 4,
	};
}

#endif /* LIB_BUS_SRC_SOURCETYPE_HPP_ */
