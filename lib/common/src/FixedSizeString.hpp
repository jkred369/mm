/*
 * FixedSizeString.hpp
 *
 *  Created on: Nov 5, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_FIXEDSIZESTRING_HPP_
#define LIB_COMMON_SRC_FIXEDSIZESTRING_HPP_

#include <algorithm>
#include <cstring>
#include <string>

namespace mm
{
	//
	// This class is a fixed size string to avoid allocating heap memory and wraps common routines for strings.
	//
	template<size_t size> class FixedSizeString
	{
	public:

		//
		// Default constructor.
		//
		FixedSizeString() : length(0)
		{
		}

		//
		// Constructor.
		//
		// string : The raw c string.
		//
		FixedSizeString(const char* string) : length(std::min(size, std::strlen(string)))
		{
			std::strncpy(buffer, string, length);
		}

		//
		// Constructor.
		//
		// string : The cpp string.
		//
		FixedSizeString(const std::string& string) : length(std::min(size, string.size()))
		{
			std::strncpy(buffer, string.c_str(), length);
		}

		//
		// Assignment from raw c string.
		//
		// string : The raw c string.
		//
		FixedSizeString<size>& operator = (const char* string)
		{
			length = std::min(size, std::strlen(string));
			std::strncpy(buffer, string, length);

			return *this;
		}

		//
		// Assignment from cpp string.
		//
		// string : The cpp string.
		//
		FixedSizeString<size>& operator = (const std::string& string)
		{
			length = std::min(size, string.size());
			std::strncpy(buffer, string, length);

			return *this;
		}

		//
		// Get the value into a cpp string.
		//
		// return : A std string with identical content on stack.
		//
		std::string toString() const
		{
			return std::string(buffer, length);
		}

		//
		// Get the value into a c style char buffer.
		//
		// return : A char buffer with size same as the length and content identical on heap.
		//
		char* toStr() const
		{
			char* result = new char[length];
			std::strncpy(result, buffer, length);

			return result;
		}

		//
		// Get the value into a c style char buffer.
		//
		// buffer : The buffer. Caller needs to make sure the length of buffer is enough.
		//
		void copy(char* buffer) const
		{
			std::strncpy(buffer, this->buffer, length);
		}

		//
		// Get the size of the string.
		//
		// return : The size of the string stored.
		//
		size_t stringSize() const
		{
			return length;
		}

		//
		// Get the capacity of the string.
		//
		// return : The capacity of the string.
		//
		size_t capacity() const
		{
			return size;
		}

	private:

		// The current effective length of the string.
		size_t length;

		// Actual storage
		char buffer[size];
	};
}



#endif /* LIB_COMMON_SRC_FIXEDSIZESTRING_HPP_ */
