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

		}

		//
		// consumer an execution message.
		//
		// message : The execution message.
		//
		void consume(const std::shared_ptr<const ExecutionMessage>& message)
		{
		}

	private:

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
