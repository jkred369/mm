/*
 * ExchangeType.hpp
 *
 *  Created on: Sep 27, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_EXCHANGE_HPP_
#define LIB_MESSAGING_SRC_EXCHANGE_HPP_

#include <cstdint>

namespace mm
{
	//
	// This enum defines the exchanges.
	//
	enum class Exchange : std::int64_t
	{
		// Shanghai stock exchange
		SSE = 0,

		// Shanghai future exchange
		SHFE = 1,
	};
}



#endif /* LIB_MESSAGING_SRC_EXCHANGE_HPP_ */
