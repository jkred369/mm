/*
 * ExecutionMessage.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_EXECUTIONMESSAGE_HPP_
#define LIB_MESSAGING_SRC_EXECUTIONMESSAGE_HPP_

#include <cstdint>

#include <Message.hpp>
#include <OrderStatus.hpp>
#include <Side.hpp>

namespace mm
{
	//
	// This message represents an execution.
	//
	class ExecutionMessage : public Message
	{
	public:

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

		// The current order status.
		OrderStatus status;

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
	};
}



#endif /* LIB_MESSAGING_SRC_EXECUTIONMESSAGE_HPP_ */
