/*
 * ExecutionMessage.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_TRADEMESSAGE_HPP_
#define LIB_MESSAGING_SRC_TRADEMESSAGE_HPP_

#include <cstdint>

#include "Message.hpp"
#include "OrderStatus.hpp"
#include "Side.hpp"

namespace mm
{
	//
	// This message represents an execution.
	//
	class TradeMessage : public Message
	{
	public:

		//
		// Default constructor.
		//
		TradeMessage();

		//
		// Check for equality of execution.
		//
		// rhs : The other message.
		//
		// return : True if the other message is identical.
		//
		bool equals(const TradeMessage& rhs) const;

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

			buffer << orderId << instrumentId << executionId << side << qty << price << status;
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

			buffer >> orderId >> instrumentId >> executionId >> side >> qty >> price >> status;
			return buffer.getError();
		}

		//
		// Determine if the message contains a trade.
		//
		// return : true if the message contains a trade.
		//
		inline bool isTrade() const
		{
			return executionId != 0;
		}

		//
		// Get the trade notional.
		//
		// return : trade notional as qty * price.
		//
		inline double notional() const
		{
			return qty * price;
		}

		// The order ID.
		std::int64_t orderId;

		// The instrument ID.
		std::int64_t instrumentId;

		// The trade ID.
		std::int64_t executionId;

		// The order side.
		Side side;

		// Trade qty.
		std::int64_t qty;

		// Trade price.
		double price;
	};

	DEFINE_OPERATORS(TradeMessage)
}



#endif /* LIB_MESSAGING_SRC_TRADEMESSAGE_HPP_ */
