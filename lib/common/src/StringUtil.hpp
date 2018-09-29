/*
 * StringUtil.hpp
 *
 *  Created on: Sep 17, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_STRINGUTIL_HPP_
#define LIB_COMMON_SRC_STRINGUTIL_HPP_

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <locale>
#include <string>

#include <fmt/format.h>

#include <NativeDefinition.hpp>

namespace mm
{
	//
	// The string utility functions.
	//
	struct StringUtil
	{
		// string representation for inf.
		static constexpr char* FP_INF_STRING = "Inf";

		// string representation for nan.
		static constexpr char* FP_NAN_STRING = "NaN";

		//
		// trim the input string from start.
		//
		// s : The string to trim.
		//
		static inline void ltrim(std::string &s)
		{
		    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		        return !std::isspace(ch);
		    }));
		}

		//
		// trim the input string from end.
		//
		// s : The string to trim.
		//
		static inline void rtrim(std::string &s)
		{
		    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		        return !std::isspace(ch);
		    }).base(), s.end());
		}

		//
		// trim the input string from both ends.
		//
		// s : The string to trim.
		//
		static inline void trim(std::string &s)
		{
		    ltrim(s);
		    rtrim(s);
		}

		//
		// Copy at most count characters from src to dest. Append '\0' to end of dest regardlessly.
		//
		// dest : The destination.
		// src : The source string.
		// count : max count of chars to copy.
		//
		static inline void copy(char* dest, const std::string& src, const std::size_t count)
		{
			const size_t size = std::min(src.size() + 1, count);
			std::memcpy(dest, src.c_str(), size);

			if (UNLIKELY(size == count))
			{
				dest[count - 1] = '\0';
			}
		}

		//
		// Fill the dest buffer with the string representation of the integer.
		//
		// dest : The destination.
		// value : The integer value.
		// count : Max count of chars to copy.
		//
		// return : length of the char written. 0 in case buffer isn't long enough.
		//
		static inline std::size_t fromInt(char* dest, const std::int64_t value, const std::size_t count)
		{
			fmt::format_int format(value);

			if (LIKELY(format.size() < count))
			{
				std::strcpy(dest, format.c_str());
				return format.size() + 1;
			}
			else
			{
				return 0;
			}
		}

		//
		// Fill the dest buffer with the string representation of the double.
		//
		// dest : The destination.
		// value : The double value.
		// count : Max count of chars to copy.
		//
		// return : Count of number of chars written to dest.
		//
		template<std::size_t Precision=6> static inline std::size_t fromDouble(char* dest, const double value, const std::size_t count)
		{
			static constexpr int FACTOR = std::pow(10, Precision);
			static constexpr char* STRINGS[] = {FP_NAN_STRING, FP_INF_STRING};

			const int type = std::fpclassify(value);

			switch (type)
			{
				case FP_NAN:
				case FP_INFINITE:
				{
					if (LIKELY(count >= std::strlen(STRINGS[type])))
					{
						std::memcpy(dest, &STRINGS[type], std::strlen(STRINGS[type]));
						return std::strlen(STRINGS[type]);
					}
					else
					{
						return 0;
					}
				}
				case FP_ZERO:
				{
					// write this separately coz 0 is quite common
					if (LIKELY(count > 0))
					{
						dest[0] = '0';
						return 1;
					}

					return 0;
				}
				case FP_SUBNORMAL:
				case FP_NORMAL:
				{
					char* buffer = dest;

					std::int64_t i = (std::int64_t) std::lrint(value * FACTOR);
					std::int64_t ai = (std::int64_t) std::abs(i);

					const fmt::format_int integral(i / FACTOR);
					const std::size_t integralSize = integral.size();

					if (LIKELY(integralSize + Precision + 1) <= count)
					{
						// copy over the integral part
						std::memcpy(buffer, integral.data(), integralSize);
						buffer += integralSize;

						// this branch must be here for precision == 0 case
						// fortunately it can be optimized away
						if (LIKELY(Precision > 0))
						{
							// append decimal point
							*buffer = '.';
							++buffer;

							// get the fractional part
							const fmt::format_int fraction(ai % FACTOR);
							const std::size_t fractionSize = fraction.size();

							// adding 1 more branch for safety
							if (LIKELY(Precision >= fractionSize))
							{
								// prepend the 0s
								std::memset(buffer, '0', Precision - fractionSize);
								buffer += Precision - fractionSize;

								// get the fraction
								std::memcpy(buffer, fraction.data(), fractionSize);
								buffer += fractionSize;
							}
						}
					}

					return buffer - dest;
				}
				default:
					return 0;
			}
		}
	};
}



#endif /* LIB_COMMON_SRC_STRINGUTIL_HPP_ */
