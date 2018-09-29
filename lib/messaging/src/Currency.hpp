/*
 * Currency.hpp
 *
 *  Created on: Sep 28, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_CURRENCY_HPP_
#define LIB_MESSAGING_SRC_CURRENCY_HPP_

#include <cstdint>

namespace mm
{
	//
	// This enum defines all the currencies in the system.
	//
	enum class Currency : std::int64_t
	{
		// Hong Kong dollar
		HKD = 0,

		// Chinese RMB
		CNY = 1,

		// USD
		USD = 2,

		// Japanese Yen
		JPY = 3,

		// Euro
		EUR = 4,
	};
}



#endif /* LIB_MESSAGING_SRC_CURRENCY_HPP_ */
