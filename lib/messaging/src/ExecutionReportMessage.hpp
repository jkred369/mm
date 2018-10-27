/*
 * ExecutionReportMessage.hpp
 *
 *  Created on: Oct 6, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_EXECUTIONREPORTMESSAGE_HPP_
#define LIB_MESSAGING_SRC_EXECUTIONREPORTMESSAGE_HPP_

#include "FieldDefinition.hpp"
#include "Message.hpp"
#include "OffsetType.hpp"
#include "OrderStatus.hpp"
#include "OrderType.hpp"
#include "Side.hpp"

namespace mm
{
	//
	// This class models an execution report for exchange connection feedback.
	//
	class ExecutionReportMessage : public Message
	{
	public:

		//
		// Default constructor.
		//
		ExecutionReportMessage();

		//
		// Check for equalness of execution report message.
		//
		// rhs : The other message.
		//
		// return : True if the other message is identical.
		//
		bool equals(const ExecutionReportMessage& rhs) const;

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

			buffer << orderId << instrumentId << openQty << tradedQty << execQty;
			buffer << price << execPrice << status << side << tradeTimestamp << executionId;

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

			buffer >> orderId >> instrumentId >> openQty >> tradedQty >> execQty;
			buffer >> price >> execPrice >> status >> side >> tradeTimestamp >> executionId;

			return buffer.getError();
		}

		//
		// Tell if the execution report contains a trade.
		//
		// return : true if the message contains a trade.
		//
		inline bool hasTrade() const
		{
			return execQty != 0;
		}

		//
		// Get the remain qty of the order.
		//
		// return : remain qty as total qty - traded qty.
		//
		inline std::int64_t totalQty() const
		{
			return openQty + tradedQty;
		}

		// The order ID.
		std::int64_t orderId;

		// The instrument ID.
		std::int64_t instrumentId;

		// The open live qty.
		std::int64_t openQty;

		// The traded qty.
		std::int64_t tradedQty;

		// The executed qty (if this is a trade).
		std::int64_t execQty;

		// The open price.
		double price;

		// The execution price (if this is a trade).
		double execPrice;

		// The order status.
		OrderStatus status;

		// The order side.
		Side side;

		// The trade timestamp if this is a trade
		DateTime tradeTimestamp;

		// The execution ID.
		ExecutionIdType executionId;
	};

	DEFINE_OPERATORS(ExecutionReportMessage)
}


#endif /* LIB_MESSAGING_SRC_EXECUTIONREPORTMESSAGE_HPP_ */
