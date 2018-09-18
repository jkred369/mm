/*
 * OrderMessage.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_ORDERMESSAGE_HPP_
#define LIB_MESSAGING_SRC_ORDERMESSAGE_HPP_

#include <cstdint>

#include "Message.hpp"
#include "OrderStatus.hpp"
#include "Side.hpp"

namespace mm
{
	//
	// This class defines an order message used to manipulate order.
	//
	class OrderMessage : public Message
	{
	public:

		// The order ID.
		std::int64_t orderId;

		// The instrument ID.
		std::int64_t instrumentId;

		// The order side.
		Side side;

		// The desired total qty for the order.
		std::int64_t totalQty;

		// The desired price for the order.
		double price;

		// The desired status for the order.
		OrderStatus status;
	};
}



#endif /* LIB_MESSAGING_SRC_ORDERMESSAGE_HPP_ */
