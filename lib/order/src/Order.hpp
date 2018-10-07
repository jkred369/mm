/*
 * Order.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDER_HPP_
#define LIB_ORDER_SRC_ORDER_HPP_

#include <memory>

#include <EnumType.hpp>
#include <ExecutionReportMessage.hpp>
#include <IPublisher.hpp>
#include <Logger.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>
#include <TradeMessage.hpp>

namespace mm
{
	//
	// The order class for order status management and actions.
	//
	template<typename ExchangeInterface, typename SummaryPool, typename TradePool> class Order
	{
	public:

		//
		// Constructor.
		//
		// summaryPool : The pool for summary objects.
		// tradePool : The pool for trade objects.
		// publisher : The publisher for order summary.
		// tradePublisher : The publisher for trade.
		// exchange : The exchange.
		//
		Order(ExchangeInterface* exchange,
				SummaryPool* summaryPool,
				TradePool* tradePool,
				IPublisher<OrderSummaryMessage>* publisher,
				IPublisher<TradeMessage>* tradePublisher) :
			exchange(*exchange),
			summaryPool(*summaryPool),
			tradePool(*tradePool),
			publisher(*publisher),
			tradePublisher(*tradePublisher),
			orderId(0),
			instrumentId(0),
			side(Side::BID),
			totalQty(0),
			tradedQty(0),
			price(0.0),
			avgTradedPrice(0.0),
			status(OrderStatus::NEW)
		{
		}

		//
		// consume the given order message to the order.
		//
		// message : The order message.
		//
		void consume(const std::shared_ptr<const OrderMessage>& message)
		{
			bool processed = false;

			// sanity check
			if (status != OrderStatus::NEW && orderId != message->orderId)
			{
				LOGERR("Invalid order messgae {} on order {}", message->orderId, orderId);
				return;
			}

			// status management
			if (status == OrderStatus::NEW)
			{
				if (message->status == OrderStatus::NEW || message->status == OrderStatus::LIVE)
				{
					// orderId == 0 and instrumentId == 0 means the order data isn't loaded
					if (orderId == 0 && instrumentId == 0)
					{
						orderId = message->orderId;
						instrumentId = message->instrumentId;
						side = message->side;
						totalQty = message->totalQty;
						price = message->price;
					}

					if (message->status == OrderStatus::LIVE)
					{
						exchange.sendOrder(message);
						status = OrderStatus::PENDING_ACK;
					}

					processed = true;
				}
			}
			else if (!OrderStatusUtil::completed(status))
			{
				if (message->status == OrderStatus::CANCELLED)
				{
					exchange.cancel(message);
					status = OrderStatus::PENDING_CANCEL;
					processed = true;
				}
			}

			// status update
			if (!processed)
			{
				LOGWARN("Ignoring order message for status {} on order status {}", toValue(message->status), toValue(status));
				return;
			}
			else if (status != OrderStatus::NEW)
			{
				sendSummary();
			}
		}

		//
		// consume an execution report message.
		//
		// message : The execution report message.
		//
		void consume(const std::shared_ptr<const ExecutionReportMessage>& message)
		{
			// sanity check
			if (UNLIKELY(orderId != message->orderId))
			{
				LOGERR("Order {} received unexpected message with order ID {}. Ignored.", orderId, message->orderId);
				return;
			}

			// firstly populate the latest status of the order
			if (LIKELY(!OrderStatusUtil::completed(status)))
			{
				avgTradedPrice = message->avgTradedPrice;
				price = message->price;
				status = message->status;
				totalQty = message->totalQty();
				tradedQty = message->tradedQty;

				sendSummary();
			}

			// if there is a trade - publish the trade as well
			if (message->hasTrade())
			{
				std::shared_ptr<TradeMessage> trade = tradePool.getShared();

				trade->orderId = orderId;
				trade->instrumentId = instrumentId;
				trade->executionId = message->executionId;
				trade->side = side;
				trade->qty = message->execQty;
				trade->price = message->execPrice;

				const Subscription sub = {SourceType::ALL, DataType::TRADE, trade->executionId};
				tradePublisher.publish(sub, trade);
			}
		}

		//
		// Send summary for the order.
		//
		void sendSummary()
		{
			// sanity check
			if (orderId == 0)
			{
				return;
			}

			std::shared_ptr<OrderSummaryMessage> message = summaryPool.getShared();

			message->orderId = orderId;
			message->instrumentId = instrumentId;
			message->side = side;
			message->totalQty = totalQty;
			message->tradedQty = tradedQty;
			message->price = price;
			message->avgTradedPrice = avgTradedPrice;
			message->status = status;

			const Subscription subscription = {SourceType::ALL, DataType::ORDER_SUMMARY, orderId};
			publisher.publish(subscription, message);
		}

		//
		// Get the order ID.
		//
		// return : order ID.
		//
		inline std::int64_t getOrderId() const
		{
			return orderId;
		}

		//
		// Get the instrument ID.
		//
		// return : instrument ID.
		//
		inline std::int64_t getInstrumentId() const
		{
			return instrumentId;
		}

		//
		// Get the order side.
		//
		// return : The order side.
		//
		inline Side getSide() const
		{
			return side;
		}

		//
		// Get the order total qty.
		//
		// return : total qty.
		//
		inline std::int64_t getTotalQty() const
		{
			return totalQty;
		}

		//
		// Get the order traded qty.
		//
		// return : traded qty.
		//
		inline std::int64_t getTradedQty() const
		{
			return tradedQty;
		}

		//
		// Get the order price.
		//
		// return : order price.
		//
		inline double getPrice() const
		{
			return price;
		}

		//
		// Get the order average traded price.
		//
		// return : averaged traded price.
		//
		inline double getAvgTradedPrice() const
		{
			return avgTradedPrice;
		}

		//
		// Get the order status.
		//
		// return : order status.
		//
		inline OrderStatus getStatus() const
		{
			return status;
		}

	private:

		// Logger of the class.
		static Logger logger;

		// The exchange.
		ExchangeInterface& exchange;

		// The message pool.
		SummaryPool& summaryPool;

		// The trade message pool.
		TradePool& tradePool;

		// The publisher.
		IPublisher<OrderSummaryMessage>& publisher;

		// The execution publisher.
		IPublisher<TradeMessage>& tradePublisher;

		// The order ID.
		std::int64_t orderId;

		// The instrument ID.
		std::int64_t instrumentId;

		// The order side.
		Side side;

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

template<typename ExchangeInterface, typename Pool, typename TradePool> mm::Logger mm::Order<ExchangeInterface, Pool, TradePool>::logger;

#endif /* LIB_ORDER_SRC_ORDER_HPP_ */
