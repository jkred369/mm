/*
 * EnumType.hpp
 *
 *  Created on: Sep 15, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_ENUMTYPE_HPP_
#define LIB_COMMON_SRC_ENUMTYPE_HPP_

#include <type_traits>

namespace mm
{
	//
	// Get the underlying type value for the enum for the strong typed enum classes.
	//
	// e : The strong typed enum class.
	//
	template <typename E>
	constexpr typename std::underlying_type<E>::type toValue(E e) noexcept {
		return static_cast<typename std::underlying_type<E>::type>(e);
	}

	//
	// Get the enum type value for the underlying type for the strong typed enum class.
	//
	// e : The type of the enum.
	//
	template <typename E>
	constexpr E fromValue(typename std::underlying_type<E>::type e) noexcept {
		return static_cast<E>(e);
	}
}


#endif /* LIB_COMMON_SRC_ENUMTYPE_HPP_ */
