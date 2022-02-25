/*
 * MathUtil.hpp
 *
 *  Created on: Oct 7, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_MATHUTIL_HPP_
#define LIB_COMMON_SRC_MATHUTIL_HPP_

#include <algorithm>

namespace mm
{
	struct MathUtil
	{
		// The epsilon inverse.
		static constexpr double E_INV = 10000000;

		// The epsilon used.
		static constexpr double E = 1 / E_INV;

		// The double compare epsilon - should be more than the rounding epsilon
		static constexpr double E_CMP = E * 10;

		//
		// Round the value (always up) to avoid the floating point error issue.
		//
		// value : The value to round.
		//
		// return : The value always rounded up.
		//
		static inline double round(double value)
		{
			return (value * E_INV + 1) * E;
		}

		//
		// Compare 2 double variables with epsilon error term.
		//
		// lhs : left side var
		// rhs : right side var
		// e : error terms.
		//
		// return : true if 2 values are differed by less than e in abs amount.
		//
		static inline bool equals(double lhs, double rhs, double e = E_CMP)
		{
			return std::abs(lhs - rhs) < e;
		}

		//
		// Check if lhs < rhs by significant amount.
		//
		// lhs : left side var
		// rhs : right side var
		// e : error terms.
		//
		// return : true if lhs < rhs by significant amount.
		//
		static inline bool less_than(double lhs, double rhs, double e = E_CMP)
		{
			return rhs - lhs > E_CMP;
		}

		//
		// Check if lhs > rhs by significant amount.
		//
		// lhs : left side var
		// rhs : right side var
		// e : error terms.
		//
		// return : true if lhs > rhs by significant amount.
		//
		static inline bool greater_than(double lhs, double rhs, double e = E_CMP)
		{
			return lhs - rhs > E_CMP;
		}

		//
		// Check if the double represents a valid market price
		//
		// price : The price.
		//
		// return : true if the given value can be a valid price.
		//
		static inline bool validPrice(double price)
		{
			return price > E_CMP;
		}
	};
}



#endif /* LIB_COMMON_SRC_MATHUTIL_HPP_ */
