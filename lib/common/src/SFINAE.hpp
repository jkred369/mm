/*
 * sfinae.hpp
 *
 *  Created on: Apr 1, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_SFINAE_HPP_
#define LIB_COMMON_SRC_SFINAE_HPP_

#include <type_traits>

namespace mm
{
	// here we define the common tests for SFINAEs.
	// the specific ones are implemented in class files.
	namespace sfinae
	{
		struct No {};

		template<typename T, typename Arg> No operator == (const T&, const Arg&);

		template<typename T, typename Arg = T> struct has_equal
		{
			enum { value = !std::is_same<decltype(*(T*)(0) == *(Arg*)(0)), No>::value };
		};
	}
}

#endif /* LIB_COMMON_SRC_SFINAE_HPP_ */
