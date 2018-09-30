/*
 * Message.hpp
 *
 *  Created on: Nov 6, 2017
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_MESSAGE_HPP_
#define LIB_MESSAGING_MESSAGE_HPP_

#include <cstdint>
#include <string>

#include <NativeDefinition.hpp>

#define DEFINE_OPERATORS(MessageName) \
\
	inline bool operator == (const MessageName& lhs, const MessageName& rhs) \
	{ \
		return lhs.equals(rhs); \
	} \
\
	inline bool operator != (const MessageName& lhs, const MessageName& rhs) \
	{ \
		return !(lhs == rhs); \
	} \

namespace mm
{
	//
	// The message header.
	//
	class MessageHeader
	{
	public:

		friend class Message;

		//
		// Get the environment symbol.
		//
		// return : the environment symbol.
		//
		std::int8_t getEnv() const
		{
			return value[0];
		}

		//
		// Get the message universe.
		//
		// return : the universe.
		//
		std::int8_t getUniverse() const
		{
			return value[1];
		}

		//
		// Get the message type.
		//
		// return : the message type.
		//
		std::int16_t getType() const
		{
			return *reinterpret_cast<const std::int16_t*> (value[2]);
		}

		//
		// The size of the message buffer.
		//
		// return : Size of the message buffer.
		//
		std::size_t getSize() const
		{
			return *reinterpret_cast<const std::uint32_t*> (value[4]);
		}

		//
		// Get the key of the message.
		//
		// return : Key of the message.
		//
		const char* getKey()
		{
			return key;
		}

	private:

		// internal buffer for the header.
		char value[8];

		// internal buffer for key.
		char key[8];
	};

	//
	// The base class for all messages.
	//
	class Message
	{
	public:

		// The CSV separator.
		static constexpr char CSV_SEPARATOR = ',';

		// The line separator.
		static constexpr char LINE_SEPARATOR = '\n';

		// The CSV separator as string.
		static const std::string CSV_SEPARATOR_STRING;

		// The line separator as string.
		static const std::string LINE_SEPARATOR_STRING;

		// The date time format as string.
		static const std::string DATETIME_FORMAT_STRING;

		// The date format as string.
		static const std::string DATE_FORMAT_STRING;

		// The time format as string.
		static const std::string TIME_FORMAT_STRING;

		//
		// The template method for serialize to buffer.
		// used to provide a uniform behavior across buffer types.
		//
		// buffer : The buffer to write to.
		//
		// return : true if the serialization done successfully.
		//
		template<typename WriteBuffer> bool serialize(WriteBuffer& buffer) const
		{
			return true;
		}

		//
		// The template method for deserialize from buffer.
		// used to provide a uniform behavrior across buffer types.
		//
		// buffer : The buffer to read from.
		//
		// return : true if the deserialization done successfully.
		//
		template<typename ReadBuffer> bool deserialize(ReadBuffer& buffer)
		{
			return true;
		}

	};
}



#endif /* LIB_MESSAGING_MESSAGE_HPP_ */
