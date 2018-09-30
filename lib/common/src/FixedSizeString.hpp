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
	template<std::size_t size> class FixedSizeString
	{
	public:

		// friend of itself
		template<std::size_t OtherSize> friend class FixedSizeString;

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
			std::strncpy(buffer, string.c_str(), length);

			return *this;
		}

		//
		// Check if the contents are equal.
		//
		// rhs : The other string.
		//
		// return : true if the contents are equal.
		//
		template<std::size_t RightSize> bool equals(const FixedSizeString<RightSize>& rhs) const
		{
			return length == rhs.length && std::memcmp(buffer, rhs.buffer, length) == 0;
		}

		//
		// Check if the contents are equal.
		//
		// rhs : The other string.
		//
		// return : true if the contents are equal.
		//
		bool equals(const FixedSizeString& rhs) const
		{
			return length == rhs.length && std::memcmp(buffer, rhs.buffer, length) == 0;
		}

		//
		// Check if the contents are equal.
		//
		// rhs : The other string.
		//
		// return : true if the contents are equal.
		//
		bool equals(const std::string& rhs) const
		{
			return length == rhs.length() && std::memcmp(buffer, rhs.c_str(), length) == 0;
		}

		//
		// generate a hash value given the content.
		//
		// return : hash value for the string.
		//
		std::size_t hash() const
		{
			std::size_t result = 7;
			for (std::size_t i = 0; i < length; ++i)
			{
				result = result * 31 + buffer[i];
			}

			return result;
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
			char* result = new char[length + 1];

			std::strncpy(result, buffer, length);
			result[length] = 0;

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
		// Get the data from the string.
		//
		// return : The data from the string.
		//
		const char* data() const
		{
			return buffer;
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

	template<std::size_t LeftSize, std::size_t RightSize> bool operator == (const FixedSizeString<LeftSize> lhs, const FixedSizeString<RightSize>& rhs)
	{
		return lhs.equals(rhs);
	}

	template<std::size_t Size> bool operator == (const FixedSizeString<Size> lhs, const std::string& rhs)
	{
		return lhs.equals(rhs);
	}

}

namespace std
{
	template<std::size_t Size> struct hash<mm::FixedSizeString<Size> >
	{
		std::size_t operator()(const mm::FixedSizeString<Size>& value) const
		{
			return value.hash();
		}
	};
}


#endif /* LIB_COMMON_SRC_FIXEDSIZESTRING_HPP_ */
