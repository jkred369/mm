/*
 * DispatchKey.hpp
 *
 *  Created on: Sep 15, 2018
 *      Author: suoalex
 */

#ifndef LIB_THREADING_SRC_DISPATCHKEY_HPP_
#define LIB_THREADING_SRC_DISPATCHKEY_HPP_

#include <cstdint>

namespace mm
{
	typedef std::int64_t KeyType;

	//
	// This class defines all the dispatch keys. Custom values can be added.
	//
	struct DispatchKey
	{
		// market data queue.
		static constexpr KeyType MARKET_DATA = 0;

		// algo queue.
		static constexpr KeyType ALGO = 1;

		// order queue.
		static constexpr KeyType ORDER = 2;
	};
}



#endif /* LIB_THREADING_SRC_DISPATCHKEY_HPP_ */
