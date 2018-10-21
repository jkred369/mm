/*
 * PositionMessage.hpp
 *
 *  Created on: Oct 21, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_POSITIONMESSAGE_HPP_
#define LIB_MESSAGING_SRC_POSITIONMESSAGE_HPP_

#include <cstdint>

#include "Message.hpp"

namespace mm
{
	//
	// This class represents a position on a specific instrument.
	//
	class PositionMessage : public Message
	{
	public:

		//
		// Default constructor.
		//
		PositionMessage();

		//
		// Check for equalness of order summary message.
		//
		// rhs : The other message.
		//
		// return : True if the other message is identical.
		//
		bool equals(const PositionMessage& rhs) const;

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
			if (UNLIKELY(!Message::serialize(buffer)))
			{
				return false;
			}

			buffer << instrumentId << totalQty << sodQty << dailyBoughtQty << dailySoldQty;
			return buffer.getError();
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
			if (UNLIKELY(!Message::deserialize(buffer)))
			{
				return false;
			}

			buffer >> instrumentId >> totalQty >> sodQty >> dailyBoughtQty >> dailySoldQty;
			return buffer.getError();
		}

		//
		// Get the remain qty of the order.
		//
		// return : remain qty as total qty - traded qty.
		//
		inline std::int64_t dailyQty() const
		{
			return dailyBoughtQty + dailySoldQty;
		}

		// The instrument ID.
		std::int64_t instrumentId;

		// The current total qty.
		std::int64_t totalQty;

		// The SOD qty.
		std::int64_t sodQty;

		// The daily bought qty. Always positive.
		std::int64_t dailyBoughtQty;

		// The daily sold qty. Always negative.
		std::int64_t dailySoldQty;
	};

	DEFINE_OPERATORS(PositionMessage)
}



#endif /* LIB_MESSAGING_SRC_POSITIONMESSAGE_HPP_ */
