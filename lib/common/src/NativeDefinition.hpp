/*
 * NativeDefinition.hpp
 *
 *  Created on: Nov 4, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_NATIVEDEFINITION_HPP_
#define LIB_COMMON_SRC_NATIVEDEFINITION_HPP_

#include <cstdint>

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define INT std::int64_t
#else
#define INT std::int32_t
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define INT std::int64_t
#else
#define INT std::int32_t
#endif
#endif

#endif /* LIB_COMMON_SRC_NATIVEDEFINITION_HPP_ */
