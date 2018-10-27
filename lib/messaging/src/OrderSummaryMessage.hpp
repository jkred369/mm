/*
 * OrderSummaryMessage.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_ORDERSUMMARYMESSAGE_HPP_
#define LIB_MESSAGING_SRC_ORDERSUMMARYMESSAGE_HPP_

#include <cstdint>

#include "Message.hpp"
#include "OrderStatus.hpp"
#include "Side.hpp"

namespace mm
{
	//
	// This class defines an order summary message.
	//
	class OrderSummaryMessage : public Message
	{
	public:

		//
		// Default constructor.
		//
		OrderSummaryMessage();

		//
		// Check for equalness of order summary message.
		//
		// rhs : The other message.
		//
		// return : True if the other message is identical.
		//
		bool equals(const OrderSummaryMessage& rhs) const;

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

			buffer << orderId << instrumentId << strategyId << side << totalQty << price << tradedNotional << status;
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

			buffer >> orderId >> instrumentId >> strategyId >> side >> totalQty >> price >> tradedNotional >> status;
			return buffer.getError();
		}

		//
		// Get the remain qty of the order.
		//
		// return : remain qty as total qty - traded qty.
		//
		inline std::int64_t remainQty() const
		{
			return totalQty - tradedQty;
		}

		//
		// Get the average traded price.
		//
		// return : the average traded price if the order is traded; or 0 if there is no trade.
		//
		inline double avgTradedPrice() const
		{
			return tradedQty == 0 ? 0.0 : tradedNotional / tradedQty;
		}

		// The order ID.
		std::int64_t orderId;

		// The instrument ID.
		std::int64_t instrumentId;

		// The strategy ID.
		std::int64_t strategyId;

		// The ordr side.
		Side side;

		// Total qty of the order.
		std::int64_t totalQty;

		// Traded qty of the order.
		std::int64_t tradedQty;

		// Market price.
		double price;

		// Total traded notional.
		double tradedNotional;

		// current order status.
		OrderStatus status;
	};

	DEFINE_OPERATORS(OrderSummaryMessage)

}



#endif /* LIB_MESSAGING_SRC_ORDERSUMMARYMESSAGE_HPP_ */
