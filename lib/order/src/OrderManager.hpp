/*
 * OrderManager.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDERMANAGER_HPP_
#define LIB_ORDER_SRC_ORDERMANAGER_HPP_

#include <memory>

#include "Order.hpp"
#include "OrderCache.hpp"

namespace mm
{
	//
	// This class is the generic order manager.
	//
	template<typename Publisher, typename ExchangeInterface, typename Pool> class OrderManager
	{
	public:

		// The exchange order type.
		typedef Order<Publisher, ExchangeInterface, Pool> ExchangeOrder;

		//
		// Constructor.
		//
		// publisher : The publisher.
		// exchange : The exchange interface.
		//
		OrderManager(const std::shared_ptr<Publisher> publisher, const std::shared_ptr<ExchangeInterface> exchange) :
			publisher(publisher), exchange(exchange)
		{
		}

		//
		// consumer a order message.
		//
		// message : The order message.
		//
		void consume(const std::shared_ptr<const OrderMessage>& message)
		{
			if (message->status == OrderStatus::NEW)
			{
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
		void consume(const std::shared_ptr<const ExecutionMessage>& message)
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

		// The order summary publisher.
		const std::shared_ptr<Publisher> publisher;

		// The exchange interface.
		const std::shared_ptr<ExchangeInterface> exchange;

		// The live orders.
		OrderCache<ExchangeOrder> liveCache;

		// The completed orders.
		OrderCache<ExchangeOrder> completedCache;
	};
}

#endif /* LIB_ORDER_SRC_ORDERMANAGER_HPP_ */
