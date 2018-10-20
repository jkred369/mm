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
		SSE = 1,

		// Shenzhen stock exchange
		SZSE = 2,

		// China financial future exchange
		CFFEX = 3,

		// Shanghai future exchange
		SHFE = 4,

		// Shenzhou commodity exchange
		CZCE = 5,

		// Dalian commodity exchange
		DCE = 6,

		// International energy exchange
		INE = 7,

		// Shanghai gold exchange
		SGE = 8,

		// Hong Kong exchange
		HKEX = 9,

		// Interactive broker
		SMART = 10,

		// Chicago commodity exchange
		GLOBEX = 11,

		// Interactive broker FX
		IDEALPRO = 12,

		// Oanda FX
		OANDA = 13,
	};
}



#endif /* LIB_MESSAGING_SRC_EXCHANGE_HPP_ */
