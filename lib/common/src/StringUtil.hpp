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
		// return : Flag if the conversion is done successfully.
		//
		static inline bool fromInt(char* dest, const std::int64_t value, const std::size_t count)
		{
			fmt::format_int format(value);

			if (LIKELY(format.size() < count))
			{
				std::strcpy(dest, format.c_str());
				return true;
			}
			else
			{
				return false;
			}
		}

		//
		// Fill the dest buffer with the string representation of the double.
		//
		// dest : The destination.
		// value : The double value.
		// count : Max count of chars to copy.
		//
		// return : Flag if the conversion is done successfully.
		//
		template<std::size_t Precision=6> static inline bool fromDouble(char* dest, const double value, const std::size_t count)
		{
			static constexpr int DIVISOR = std::pow(10, Precision);
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
						return true;
					}
					else
					{
						return false;
					}
				}
				case FP_ZERO:
				{
					// write this separately coz 0 is quite common
					if (LIKELY(count > 0))
					{
						dest[0] = '0';
						return true;
					}

					return false;
				}
				case FP_SUBNORMAL:
				case FP_NORMAL:
				{
					long integral = (long) value;
					long fractional = std::lrint((value - integral) * DIVISOR);

					fmt::format_int formatIntegral(integral);
					if (LIKELY(formatIntegral.size() + Precision + 1) <= count)
					{
						fmt::format_int formatFractional(fractional);

						if (LIKELY(formatFractional.size() <= Precision))
						{
							// write the content
							char* buffer = dest;
							std::memcpy(buffer, formatIntegral.data(), formatIntegral.size());
							buffer += formatIntegral.size();

							buffer = '.';
							buffer += 1;

							std::memset(buffer, '0', Precision - formatFractional.size());
							buffer += Precision - formatFractional.size();

							std::memcpy(buffer, formatFractional.data(), formatFractional.size());
							return true;
						}
					}

					return false;
				}
				default:
					return false;
			}
		}
	};
}



#endif /* LIB_COMMON_SRC_STRINGUTIL_HPP_ */
