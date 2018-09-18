/*
 * OrderStatus.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_ORDERSTATUS_HPP_
#define LIB_MESSAGING_SRC_ORDERSTATUS_HPP_

namespace mm
{
	//
	// The order status enum.
	//
	enum class OrderStatus : int
	{
		// new order
		NEW = 0,

		// order sent pending ack from exchange
		PENDING_ACK = 1,

		// order live in market
		LIVE = 2,

		// order amend sent pending ack from exchange
		PENDING_AMEND = 3,

		// order cancel sent pending ack from exchange
		PENDING_CANCEL = 4,

		// order cancelled and terminated
		CANCELLED = 5,

		// order fully filled and completed
		FULL_FILLED = 6,
	};
}



#endif /* LIB_MESSAGING_SRC_ORDERSTATUS_HPP_ */
