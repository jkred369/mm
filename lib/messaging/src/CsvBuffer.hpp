/*
 * CsvBuffer.hpp
 *
 *  Created on: Sep 28, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_CSVBUFFER_HPP_
#define LIB_MESSAGING_SRC_CSVBUFFER_HPP_

#include <algorithm>

#include <EnumType.hpp>
#include <FixedSizeString.hpp>
#include <Logger.hpp>
#include <Message.hpp>
#include <StringUtil.hpp>

namespace mm
{
	//
	// This class implements a buffer concept supporting input and output.
	//
	template<std::size_t size = 1024> class CsvBufferT
	{
	public:

		//
		// Default constructor.
		//
		CsvBufferT()
		{
		}

		//
		// Constructor with the buffer as input.
		//
		// input : The char sequence as input.
		//
		CsvBufferT(const char* input)
		{
			const std::size_t inputSize = std::strlen(input);
			if (UNLIKELY(inputSize >= size))
			{
				LOGERR("C-string with size {} too long to fit in buffer of size {}", inputSize, size);
				return;
			}

			std::memcpy(buffer, input, inputSize + 1);
		}

		//
		// Constructor with string as input.
		//
		// input : The string as input.
		//
		CsvBufferT(const std::string& input)
		{
			const std::size_t inputSize = input.size();
			if (UNLIKELY(inputSize >= size))
			{
				LOGERR("String with size {} too long to fit in buffer of size {}", inputSize, size);
				return;
			}

			std::memcpy(buffer, input.c_str(), inputSize + 1);
		}

		//
		// Assign the content of the buffer from input char buffer.
		//
		// input : The input buffer.
		//
		CsvBufferT& operator = (const char* input)
		{
			const std::size_t inputSize = std::strlen(input);
			if (LIKELY(inputSize < size))
			{
				std::memcpy(buffer, input, inputSize + 1);

				writePtr = buffer;
				readPtr = buffer;
			}

			return *this;
		}

		//
		// Assign the content of the buffer from input string.
		//
		// input : The input string.
		//
		CsvBufferT& operator = (const std::string& input)
		{
			const std::size_t inputSize = input.size();
			if (LIKELY(inputSize < size))
			{
				std::memcpy(buffer, input, inputSize + 1);

				writePtr = buffer;
				readPtr = buffer;
			}

			return *this;
		}

		//
		// output an int32 to the buffer.
		//
		// value : The int32 value.
		//
		CsvBufferT& operator << (std::int32_t value)
		{
			writePtr += StringUtil::fromInt(writePtr, value, remainSize());
			return *this;
		}

		//
		// output an int64 to the buffer.
		//
		// value : The int64 value.
		//
		CsvBufferT& operator << (std::int64_t value)
		{
			writePtr += StringUtil::fromInt(writePtr, value, remainSize());
			return *this;
		}

		//
		// output a double to the buffer.
		//
		// value : The double value;
		//
		CsvBufferT& operator << (double value)
		{
			writePtr += StringUtil::fromDouble(writePtr, value, remainSize());
			return *this;
		}

		//
		// output a string to the buffer.
		//
		// value : The string.
		//
		CsvBufferT& operator << (const std::string& value)
		{
			const std::size_t inputSize = value.size();
			if (LIKELY(inputSize >= size))
			{
				std::memcpy(writePtr, value.c_str(), inputSize + 1);
				writePtr += inputSize + 1;
			}

			return *this;
		}

		//
		// output an enum type to the buffer.
		//
		// value : The enum type.
		//
		template<typename EnumType> CsvBufferT& operator << (EnumType value)
		{
			return operator << (toValue(value));
		}

		//
		// output a fixed size string to the buffer.
		//
		// value : The fixed size string.
		//
		template<std::size_t size> CsvBufferT& operator << (FixedSizeString<size> value)
		{
			if (LIKELY(value.stringSize() < remainSize()))
			{
				std::memcpy(writePtr, value.data(), value.stringSize());
				writePtr += value.stringSize();
			}

			return *this;
		}

		//
		// input an int32 from the buffer.
		//
		// value : The int32 value.
		//
		CsvBufferT& operator >> (std::int32_t& value)
		{
			char* pos = std::strchr(readPtr, Message::CSV_SEPARATOR);
			if (LIKELY(pos != nullptr))
			{
				// a bit hacky but works and fast
				char temp = *pos;

				*pos = '\0';
				value = std::atoi(readPtr);
				*pos = temp;
			}

			return *this;
		}

		//
		// input an int64 from the buffer.
		//
		// value : The int64 value.
		//
		CsvBufferT& operator >> (std::int64_t& value)
		{
			char* pos = std::strchr(readPtr, Message::CSV_SEPARATOR);
			if (LIKELY(pos != nullptr))
			{
				// a bit hacky but works and fast
				char temp = *pos;

				*pos = '\0';
				value = std::atoi(readPtr);
				*pos = temp;
			}

			return *this;
		}

		//
		// input a double from the buffer.
		//
		// value : The double value.
		//
		CsvBufferT& operator >> (double& value)
		{
			char* pos = std::strchr(readPtr, Message::CSV_SEPARATOR);
			if (LIKELY(pos != nullptr))
			{
				// a bit hacky but works and fast
				char temp = *pos;

				*pos = '\0';
				value = std::atof(readPtr);
				*pos = temp;
			}

			return *this;
		}

		//
		// input a string from the buffer.
		//
		// value : The string value.
		//
		CsvBufferT& operator >> (std::string& value)
		{
			char* pos = std::strchr(readPtr, Message::CSV_SEPARATOR);
			if (LIKELY(pos != nullptr))
			{
				// a bit hacky but works and fast
				char temp = *pos;

				*pos = '\0';
				value.append(readPtr);
				*pos = temp;
			}

			return *this;
		}

		//
		// input an enum type from the buffer.
		//
		// value : The enum.
		//
		template<typename EnumType> CsvBufferT& operator >> (EnumType& value)
		{
			typename std::underlying_type<E>::type underlyingValue;
			operator >> (underlyingValue);

			value = fromValue(underlyingValue);
			return *this;
		}

		//
		// input a fixed size string from the buffer.
		//
		// value : The fixed size string.
		//
		template<std::size_t size> CsvBufferT& operator >> (FixedSizeString<size>& value)
		{
			char* pos = std::strchr(readPtr, Message::CSV_SEPARATOR);
			pos = pos == nullptr ? buffer + size : pos;

			// a bit hacky but works and fast
			char temp = *pos;

			*pos = '\0';
			value.append(readPtr);
			*pos = temp;

			return *this;
		}

	private:

		//
		// Get the remain size of the buffer for write.
		//
		// return : The remain size of the buffer.
		//
		inline std::size_t remainSize() const
		{
			return writePtr < buffer + size ? buffer + size - writePtr : 0;
		}

		//
		// Flag if error generated during reading.
		//
		// return : true if error generated during reading.
		//
		inline bool getError() const
		{
			return readError;
		}

		//
		// attempt to copy the char sequence from source with the given size.
		//
		// source : The source char array.
		// size : The size of the content.
		//
		inline CsvBufferT& copyCharSeq(const char* source, const std::size_t size)
		{
			if (LIKELY(size < remainSize()))
			{
				std::memcpy(writePtr, source, size + 1);
				writePtr += size + 1;
			}

			return *this;
		}

		// The logger of the class.
		static Logger logger;

		// The buffer.
		char buffer[size];

		// The reading pointer.
		const char* readPtr = buffer;

		// The writing pointer.
		char* writePtr = buffer;

		// The error flag for reading.
		bool readError = false;
	};

	// Define the commonly used class.
	typedef CsvBufferT<> CsvBuffer;
}

template<std::size_t size> mm::Logger mm::CsvBufferT<size>::logger;

#endif /* LIB_MESSAGING_SRC_CSVBUFFER_HPP_ */
