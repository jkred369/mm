/*
 * Runnable.hpp
 *
 *  Created on: Oct 27, 2017
 *      Author: suoalex
 */

#ifndef LIB_THREADING_SRC_RUNNABLE_HPP_
#define LIB_THREADING_SRC_RUNNABLE_HPP_

#include <functional>

namespace mm
{
	// Define the runnable type.
	typedef std::function<void()> Runnable;
}



#endif /* LIB_THREADING_SRC_RUNNABLE_HPP_ */
