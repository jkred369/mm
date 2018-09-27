/*
 * CallPutType.hpp
 *
 *  Created on: Sep 27, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_CALLPUTTYPE_HPP_
#define LIB_MESSAGING_SRC_CALLPUTTYPE_HPP_

#include <cstdint>

namespace mm
{
	//
	// This class defines the call put type.
	//
	enum class CallPutType : std::int64_t
	{
		// Call option
		CALL = 0,

		// Put option
		PUT = 1,
	};
}



#endif /* LIB_MESSAGING_SRC_CALLPUTTYPE_HPP_ */
