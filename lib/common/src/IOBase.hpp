/*
 * IOReactor.hpp
 *
 *  Created on: Feb 28, 2022
 *      Author: alexsuo
 */

#ifndef LIB_COMMON_SRC_IOBASE_HPP_
#define LIB_COMMON_SRC_IOBASE_HPP_

#include <functional>

namespace mm
{
	// Flags for IO operations
	static constexpr int READ = 1;
	static constexpr int WRITE = 2;

	// Reactor data type
	typedef std::function<void(uint32_t)> Reactor;
}

#endif /* LIB_COMMON_SRC_IOBASE_HPP_ */
