/*
 * MathUtil.hpp
 *
 *  Created on: Oct 7, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_MATHUTIL_HPP_
#define LIB_COMMON_SRC_MATHUTIL_HPP_

namespace mm
{
	struct MathUtil
	{
		// The epsilon inverse.
		static constexpr double E_INV = 1000000;

		// The epsilon used.
		static constexpr double E = 1 / E_INV;

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
	};
}



#endif /* LIB_COMMON_SRC_MATHUTIL_HPP_ */
