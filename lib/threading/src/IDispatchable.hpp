/*
 * IDispatchable.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_THREADING_SRC_IDISPATCHABLE_HPP_
#define LIB_THREADING_SRC_IDISPATCHABLE_HPP_

#include <DispatchKey.hpp>

namespace mm
{
	//
	// defines the dispatchable interface.
	//
	struct IDispatchable
	{
		virtual ~IDispatchable() {}

		//
		// Get the key for the dispatchable.
		//
		// return : The dispatch key for this object.
		//
		virtual const KeyType getKey() const = 0;
	};
}



#endif /* LIB_THREADING_SRC_IDISPATCHABLE_HPP_ */
