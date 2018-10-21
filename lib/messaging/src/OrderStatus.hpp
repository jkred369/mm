/*
 * OrderStatus.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_ORDERSTATUS_HPP_
#define LIB_MESSAGING_SRC_ORDERSTATUS_HPP_

#include <cstdint>

namespace mm
{
	//
	// The order status enum.
	//
	enum class OrderStatus : std::int64_t
	{
		// new order
		NEW = 0,

		// order sent pending ack from exchange
		PENDING_ACK = 1,

		// order live in market
		LIVE = 2,

		// new order rejected
		NEW_REJECTED = 3,

		// order cancel sent pending ack from exchange
		PENDING_CANCEL = 4,

		// order cancelled and terminated
		CANCELLED = 5,

		// order fully filled and completed
		FULL_FILLED = 6,

		// cancel rejected
		CANCEL_REJECTED = 7,
	};

	//
	// The helper class for order status.
	//
	struct OrderStatusUtil
	{
		//
		// Determine if an order status is completed - that is, no more action allowed.
		//
		// return : true if the order is completed.
		//
		static inline bool completed(const OrderStatus status)
		{
			return status == OrderStatus::CANCELLED || status == OrderStatus::FULL_FILLED ||
					status == OrderStatus::NEW_REJECTED || status == OrderStatus::CANCEL_REJECTED;
		}
	};

}



#endif /* LIB_MESSAGING_SRC_ORDERSTATUS_HPP_ */
