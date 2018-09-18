/*
 * Order.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDER_HPP_
#define LIB_ORDER_SRC_ORDER_HPP_

#include <memory>

#include <ExecutionMessage.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>

namespace mm
{
	//
	// The order class for order status management and actions.
	//
	template<typename Publisher, typename ExchangeInterface> class Order
	{
	public:

		//
		// Constructor.
		//
		// publisher : The publisher for order summary.
		// exchange : The exchange.
		//
		Order(const std::shared_ptr<Publisher>& publisher, const std::shared_ptr<ExchangeInterface>& exchange) :
			publisher(publisher),
			exchange(exchange),
			orderId(0),
			instrumentId(0),
			totalQty(0),
			tradedQty(0),
			price(0.0),
			avgTradedPrice(0.0),
			status(OrderStatus::NEW)
		{
		}

		//
		// Apply the given order message to the order.
		//
		void consume(const std::shared_ptr<const OrderMessage>& orderMessage)
		{
			bool processed = false;

			// status management
			switch (status)
			{
				case OrderStatus::NEW:
				{
					if (orderMessage->status == OrderStatus::NEW)
					{
						processed = true;

						orderId = orderMessage->orderId;
						instrumentId = orderMessage->instrumentId;
						totalQty = orderMessage->totalQty;
						price = orderMessage->price;
					}
					else if (orderMessage->status == OrderStatus::LIVE)
					{
						processed = true;

						exchange->sendOrder()
					}

					break;
				}
				case OrderStatus::LIVE:
					break;
				default:
					break;
			}

			if (!processed)
			{
				LOGWARN("Ignoring order message for status {} on order status {}", orderMessage->status, status);
			}
		}

		//
		// Send summary for the order.
		//
		void sendSummary()
		{

		}

	private:

		// The publisher.
		const std::shared_ptr<Publisher> publisher;

		// The exchange.
		const std::shared_ptr<ExchangeInterface> exchange;

		// The order ID.
		std::int64_t orderId;

		// The instrument ID.
		std::int64_t instrumentId;

		// Total qty of the order.
		std::int64_t totalQty;

		// Traded qty of the order.
		std::int64_t tradedQty;

		// Market price.
		double price;

		// Average traded price.
		double avgTradedPrice;

		// current order status.
		OrderStatus status;

	};
}



#endif /* LIB_ORDER_SRC_ORDER_HPP_ */
