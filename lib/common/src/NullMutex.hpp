/*
 * NullMutex.hpp
 *
 *  Created on: Sep 15, 2018
 *      Author: suoalex
 */

#ifndef LIB_THREADING_SRC_NULLMUTEX_HPP_
#define LIB_THREADING_SRC_NULLMUTEX_HPP_

namespace mm
{
	//
	// Implement a null mutex modeling Lockable concept.
	//
	struct NullMutex
	{
		void lock() {}
		void unlock() noexcept {}
		bool try_lock() { return true; }
	};
}

#endif /* LIB_THREADING_SRC_NULLMUTEX_HPP_ */
