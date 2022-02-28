/*
 * IOReactor.hpp
 *
 *  Created on: Feb 28, 2022
 *      Author: alexsuo
 */

#ifndef LIB_COMMON_SRC_IOREACTOR_HPP_
#define LIB_COMMON_SRC_IOREACTOR_HPP_

#include <functional>

namespace mm
{
	typedef std::function<void(uint32_t)> Reactor;
}

#endif /* LIB_COMMON_SRC_IOREACTOR_HPP_ */
