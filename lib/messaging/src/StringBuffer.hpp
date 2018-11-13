/*
 * StringBuffer.hpp
 *
 *  Created on: Sep 30, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_STRINGBUFFER_HPP_
#define LIB_MESSAGING_SRC_STRINGBUFFER_HPP_

#include <ostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <fmt/format.h>

#include <DateTime.hpp>
#include <EnumType.hpp>
#include <FixedSizeString.hpp>
#include <Logger.hpp>
#include <NativeDefinition.hpp>
#include <StringUtil.hpp>

#include "Message.hpp"

namespace mm
{
	//
	// This class implements a simple read and write buffer concept using std operations.
	//
	class StringBuffer
	{
	public:

		//
		// Default constructor.
		//
		StringBuffer() : errorFlag(false), readIndex(0)
		{
		}

		//
		// Constructor with string as input.
		//
		// input : The string as input.
		//
		StringBuffer(const std::string& input) : errorFlag(false), readIndex(0)
		{
			if (!input.empty())
			{
				boost::split(items, input, boost::is_any_of(Message::CSV_SEPARATOR_STRING));
				for (std::string& value : items)
				{
					StringUtil::trim(value);
				}
			}
		}

		//
		// Assign the content of the buffer from input string.
		//
		// input : The input string.
		//
		StringBuffer& operator = (const std::string& input)
		{
			items.clear();

			if (!input.empty())
			{
				boost::split(items, input, boost::is_any_of(Message::CSV_SEPARATOR_STRING));
				for (std::string& value : items)
				{
					StringUtil::trim(value);
				}
			}

			return *this;
		}

		//
		// output to a stream.
		//
		// so : The stream.
		//
		void output(std::ostream& os) const
		{
			for (std::size_t i = 0; i < items.size() - 1; ++i)
			{
				os << items[i] << Message::CSV_SEPARATOR;
			}

			os << items[items.size() - 1] << std::endl;
		}

		//
		// output an int32 to the buffer.
		//
		// value : The int32 value.
		//
		inline StringBuffer& operator << (std::int32_t value)
		{
			const fmt::format_int format(value);
			items.push_back(format.str());

			return *this;
		}

		//
		// output an int64 to the buffer.
		//
		// value : The int64 value.
		//
		inline StringBuffer& operator << (std::int64_t value)
		{
			const fmt::format_int format(value);
			items.push_back(format.str());

			return *this;
		}

		//
		// output an int64 to the buffer.
		//
		// value : The int64 value.
		//
		inline StringBuffer& operator << (std::uint64_t value)
		{
			const fmt::format_int format(value);
			items.push_back(format.str());

			return *this;
		}

		//
		// output a double to the buffer.
		//
		// value : The double value;
		//
		inline StringBuffer& operator << (double value)
		{
			try
			{
				items.push_back(boost::lexical_cast<std::string> (value));
			}
			catch (const boost::bad_lexical_cast& e)
			{
				errorFlag = true;
				LOGERR("Failed to write to buffer for double value {}, {}", value, e.what());
			}

			return *this;
		}

		//
		// output a string to the buffer.
		//
		// value : The string.
		//
		inline StringBuffer& operator << (const std::string& value)
		{
			items.push_back(value);
			return *this;
		}

		//
		// output a string to the buffer.
		//
		// value : The string.
		//
		inline StringBuffer& operator << (const std::string&& value)
		{
			items.push_back(value);
			return *this;
		}

		//
		// output a date to the buffer.
		//
		// value : The date.
		//
		inline StringBuffer& operator << (const Date& value)
		{
			const std::int64_t yyyymmdd = DateUtil::dateToInt(value);
			return operator << (yyyymmdd);
		}

		//
		// output a time stamp to the buffer.
		//
		// value : The time stamp.
		//
		inline StringBuffer& operator << (const DateTime& value)
		{
			return operator << (boost::posix_time::to_iso_string(value));
		}

		//
		// output an enum type to the buffer.
		//
		// value : The enum type.
		//
		template<typename EnumType> inline StringBuffer& operator << (EnumType value)
		{
			return operator << (toValue(value));
		}

		//
		// output a fixed size string to the buffer.
		//
		// value : The fixed size string.
		//
		template<std::size_t size> inline StringBuffer& operator << (FixedSizeString<size> value)
		{
			items.push_back(value.toString());
			return *this;
		}

		//
		// input an int32 from the buffer.
		//
		// value : The int32 value.
		//
		inline StringBuffer& operator >> (std::int32_t& value)
		{
			if (UNLIKELY(readIndex >= items.size()))
			{
				LOGERR("Attempt to read int32 beyond items. Index: {}, item size: {}", readIndex, items.size());
				errorFlag = true;
				return *this;
			}

			try
			{
				value = boost::lexical_cast<std::int32_t> (items[readIndex]);
			}
			catch (const boost::bad_lexical_cast& e)
			{
				errorFlag = true;
				LOGERR("Failed to read int32 value from buffer: {}, {}", items[readIndex], e.what());
			}

			++readIndex;
			return *this;
		}

		//
		// input an int64 from the buffer.
		//
		// value : The int64 value.
		//
		inline StringBuffer& operator >> (std::int64_t& value)
		{
			if (UNLIKELY(readIndex >= items.size()))
			{
				LOGERR("Attempt to read int64 beyond items. Index: {}, item size: {}", readIndex, items.size());
				errorFlag = true;
				return *this;
			}

			try
			{
				value = boost::lexical_cast<std::int64_t> (items[readIndex]);
			}
			catch (const boost::bad_lexical_cast& e)
			{
				errorFlag = true;
				LOGERR("Failed to read int32 value from buffer: {}, {}", items[readIndex], e.what());
			}

			++readIndex;
			return *this;
		}

		//
		// input an uint64 from the buffer.
		//
		// value : The uint64 value.
		//
		inline StringBuffer& operator >> (std::uint64_t& value)
		{
			if (UNLIKELY(readIndex >= items.size()))
			{
				LOGERR("Attempt to read int64 beyond items. Index: {}, item size: {}", readIndex, items.size());
				errorFlag = true;
				return *this;
			}

			try
			{
				value = boost::lexical_cast<std::uint64_t> (items[readIndex]);
			}
			catch (const boost::bad_lexical_cast& e)
			{
				errorFlag = true;
				LOGERR("Failed to read int32 value from buffer: {}, {}", items[readIndex], e.what());
			}

			++readIndex;
			return *this;
		}

		//
		// input a double from the buffer.
		//
		// value : The double value.
		//
		inline StringBuffer& operator >> (double& value)
		{
			if (UNLIKELY(readIndex >= items.size()))
			{
				LOGERR("Attempt to read double beyond items. Index: {}, item size: {}", readIndex, items.size());
				errorFlag = true;
				return *this;
			}

			try
			{
				value = boost::lexical_cast<double> (items[readIndex]);
			}
			catch (const boost::bad_lexical_cast& e)
			{
				errorFlag = true;
				LOGERR("Failed to read int32 value from buffer: {}, {}", items[readIndex], e.what());
			}

			++readIndex;
			return *this;
		}

		//
		// input a string from the buffer.
		//
		// value : The string value.
		//
		inline StringBuffer& operator >> (std::string& value)
		{
			if (UNLIKELY(readIndex >= items.size()))
			{
				LOGERR("Attempt to read string beyond items. Index: {}, item size: {}", readIndex, items.size());
				errorFlag = true;
				return *this;
			}

			value = items[readIndex];

			++readIndex;
			return *this;
		}

		//
		// input a date from the buffer.
		//
		// value : The date value.
		//
		inline StringBuffer& operator >> (Date& value)
		{
			std::int64_t yyyymmdd;
			operator >> (yyyymmdd);
			value = DateUtil::intToDate(yyyymmdd);

			return *this;
		}

		//
		// input a time stamp from the buffer.
		//
		// value : The time stamp value.
		//
		inline StringBuffer& operator >> (DateTime& value)
		{
			std::string stringValue;
			operator >> (stringValue);

			value = boost::posix_time::from_iso_string(stringValue);
			return *this;
		}

		//
		// input an enum type from the buffer.
		//
		// value : The enum.
		//
		template<typename EnumType> inline StringBuffer& operator >> (EnumType& value)
		{
			if (UNLIKELY(readIndex >= items.size()))
			{
				LOGERR("Attempt to read enum beyond items. Index: {}, item size: {}", readIndex, items.size());
				errorFlag = true;
				return *this;
			}

			typename std::underlying_type<EnumType>::type underlyingValue;
			operator >> (underlyingValue);

			value = fromValue<EnumType>(underlyingValue);
			return *this;
		}

		//
		// input a fixed size string from the buffer.
		//
		// value : The fixed size string.
		//
		template<std::size_t size> inline StringBuffer& operator >> (FixedSizeString<size>& value)
		{
			if (UNLIKELY(readIndex >= items.size()))
			{
				LOGERR("Attempt to read fix string beyond items. Index: {}, item size: {}", readIndex, items.size());
				errorFlag = true;
				return *this;
			}

			value = items[readIndex];

			++readIndex;
			return *this;
		}

		//
		// Get if any error happened.
		//
		// return : The error flag.
		//
		inline bool getError() const
		{
			return errorFlag;
		}

	private:

		// Logger of the class
		static Logger logger;

		// The current items in the buffer.
		std::vector<std::string> items;

		// The error flag.
		bool errorFlag;

		// The read index.
		std::size_t readIndex;
	};

	//
	// output to a stream.
	//
	// so : The stream.
	// buffer : The buffer object.
	//
	std::ostream& operator << (std::ostream& os, const StringBuffer& buffer);
}

#endif /* LIB_MESSAGING_SRC_STRINGBUFFER_HPP_ */
