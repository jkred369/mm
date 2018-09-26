/*
 * OrderManager.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDERMANAGER_HPP_
#define LIB_ORDER_SRC_ORDERMANAGER_HPP_

#include <memory>

#include <Dispatcher.hpp>
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
			public IService,
			public IConsumer<OrderMessage>,
			public IConsumer<ExecutionMessage>,
			public PublisherAdapter<OrderSummaryMessage>
	{
	public:

		// The exchange order type.
		typedef Order<OrderManager, ExchangeInterface, Pool> ExchangeOrder;

		//
		// Constructor.
		//
		// publisher : The publisher.
		// exchange : The exchange interface.
		//
		OrderManager(const std::shared_ptr<Dispatcher> dispatcher, const std::shared_ptr<ExchangeInterface> exchange) :
			PublisherAdapter<OrderSummaryMessage> (dispatcher),
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
				const std::shared_Ptr<ExchangeOrder>

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
		virtual void consume(const std::shared_ptr<const ExecutionMessage>& message) override
		{
			const std::shared_ptr<Order>& order = liveCache.getOrder(message->instrumentId, message->orderId);

			if (order.get() == nullptr)
			{
				LOGERR("Error getting order with ID: {}, instrument ID: {}", message->orderId, message->instrumentId);
				return;
			}

			order->consume(message);
		}

	private:

		// The object pool for orders.
		static Pool<ExchangeOrder, 1000> pool;

		// The exchange interface.
		const std::shared_ptr<ExchangeInterface> exchange;

		// The live orders.
		OrderCache<ExchangeOrder> liveCache;

		// The completed orders.
		OrderCache<ExchangeOrder> completedCache;
	};
}

#endif /* LIB_ORDER_SRC_ORDERMANAGER_HPP_ */
