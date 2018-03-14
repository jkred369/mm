/*
 * Message.hpp
 *
 *  Created on: Nov 6, 2017
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_MESSAGE_HPP_
#define LIB_MESSAGING_MESSAGE_HPP_

#include <cstdint>
#include <mutex>

#include <Recyclable.hpp>

namespace mm
{
	//
	// The message header.
	//
	class MessageHeader
	{
	public:

		template<typename ObjectType, typename Mutex> friend class Message;

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
		size_t getSize() const
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
	template<typename ObjectType, typename Mutex = std::recursive_mutex> class Message : public Recyclable<ObjectType, Mutex>
	{
	public:

		// The CSV separator.
		constexpr char CSV_SEPARATOR = ',';

		// The line separator.
		constexpr char LINE_SEPARATOR = '\n';

		// The CSV separator as string.
		constexpr std::string CSV_SEPARATOR_STRING = std::string(CSV_SEPARATOR);

		// THe line separator as string.
		constexpr std::string LINE_SEPARATOR_STRING = std::string(LINE_SEPARATOR);

		// The date time format as string.
		constexpr std::string DATETIME_FORMAT_STRING = "yyyy-MM-dd HH:mm:ss.SSS";

		// The date format as string.
		constexpr std::string DATE_FORMAT_STRING = "yyyy-MM-dd";

		// The time format as string.
		constexpr std::string TIME_FORMAT_STRING = "HH:mm:ss.SSS";

		//
		// Virtual destructor.
		//
		virtual ~Message()
		{
		}

	protected:

		// Header of the message.
		MessageHeader header;

	};
}



#endif /* LIB_MESSAGING_MESSAGE_HPP_ */
