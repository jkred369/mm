/*
 * OrderManager.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDERMANAGER_HPP_
#define LIB_ORDER_SRC_ORDERMANAGER_HPP_

#include <memory>

#include <DispatchableService.hpp>
#include <Dispatcher.hpp>
#include <ExecutionReportMessage.hpp>
#include <IConsumer.hpp>
#include <IService.hpp>
#include <PublisherAdapter.hpp>
#include <OrderMessage.hpp>

#include "Order.hpp"
#include "OrderCache.hpp"

namespace mm
{
	//
	// This class is the generic order manager implementing OrderListener concept.
	//
	template<typename ExchangeInterface, typename Pool> class OrderManager :
			public DispatchableService,
			public IConsumer<OrderMessage>,
			public PublisherAdapter<OrderSummaryMessage>,
			public PublisherAdapter<TradeMessage>
	{
	public:

		// The exchange order type.
		typedef Order<ExchangeInterface, Pool, Pool> ExchangeOrder;

		//
		// Constructor.
		//
		// publisher : The publisher.
		// exchange : The exchange interface.
		//
		OrderManager(
				Dispatcher& dispatcher,
				ExchangeInterface& exchange) :
			PublisherAdapter<OrderSummaryMessage> (dispatcher),
			PublisherAdapter<TradeMessage> (dispatcher),
			exchange(exchange)
		{
		}

		//
		// consumer a order message.
		//
		// message : The order message.
		//
		virtual void consume(const std::shared_ptr<const OrderMessage>& message) override
		{
			if (message->status == OrderStatus::NEW || message->status == OrderStatus::LIVE)
			{
				const std::shared_Ptr<ExchangeOrder> order(new Order(this, this, exchange));
				liveCache.addOrder(pool.get(publisher, exchange));
			}

			// the above add make sure there is no nullptr concern
			liveCache.getOrder(message->instrumentId, message->orderId)->consume(message);
		}

		//
		// consumer an execution message.
		//
		// message : The execution message.
		//
		virtual void consume(const std::shared_ptr<const ExecutionReportMessage>& message) override
		{
			const std::shared_ptr<Order>& order = liveCache.getOrder(message->instrumentId, message->orderId);

			if (order.get() == nullptr)
			{
				LOGERR("Error getting order with ID: {}, instrument ID: {}", message->orderId, message->instrumentId);
				return;
			}

			order->consume(message);
		}

		//
		// This is the exchange call back on any execution report.
		// TODO: review the design. should we consume raw pointer? or template the consumer pointer type?
		//
		// message : The execution report message.
		//
		void onExecutionReport(const ExecutionReportMessage& message)
		{
		}

	private:

		// The object pool for orders.
		static Pool<ExchangeOrder, 1000> pool;

		// The exchange interface.
		ExchangeInterface const* exchange;

		// The live orders.
		OrderCache<ExchangeOrder> liveCache;

		// The completed orders.
		OrderCache<ExchangeOrder> completedCache;
	};
}

#endif /* LIB_ORDER_SRC_ORDERMANAGER_HPP_ */
