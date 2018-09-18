/*
 * OrderCache.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDERCACHE_HPP_
#define LIB_ORDER_SRC_ORDERCACHE_HPP_

#include <memory>
#include <unordered_map>
#include <vector>

#include <Order.hpp>

namespace mm
{
	//
	// This class represents an order cache for all live orders in the market.
	//
	template<typename Order, typename Pool> class OrderCache
	{
	public:

		// Type of the order map.
		typedef std::vector<std::shared_ptr<Order> > OrderCollection;

		//
		// Determine if the order can potentially cross with another order.
		//
		// newOrder : The order to be sent.
		//
		bool hasCrossOrder(const std::shared_ptr<Order>& newOrder) const
		{
			for (const std::shared_ptr<Order>& order : orderByInstrumentMap[newOrder->getInstrumentId()])
			{
				if ((order->getSide() == Side::BID && newOrder->getSide() == Side::ASK && order->getPrice() >= newOrder.getPrice()) ||
					(order->getSide() == Side::ASK && newOrder->getSide() == Side::BID && order->getPrice() <= newOrder.getPrice()))
				{
					return true;
				}
			}

			return false;
		}

		//
		// Get an order from the order cache.
		//
		// instrumentId : The instrument ID.
		// orderId : The order ID.
		// create : if the order should be created in case not found.
		//
		// return : the pointer for the order.
		//
		inline const std::shared_ptr<Order>& getOrder(std::int64_t instrumentId, std::int64_t orderId, bool create = false)
		{
			OrderCollection& collection = orderByInstrumentMap[instrumentId];

			auto it = std::find_if(collection.begin(), collection.end(), [] (const std::shared_ptr<Order>& order) {
				return order->getOrderId() == orderId;
			});

			if (it != collection.end())
			{
				return *it;
			}
			else if (create)
			{
				collection.emplace_back(pool.get());
				return collection.back();
			}
			else
			{
				return std::shared_ptr<Order> ();
			}
		}

		//
		// Remove an order from the order cache.
		//
		// instrumentId : The instrument ID.
		// orderId : The order ID.
		//
		// return : the removed order; or empty pointer if the order cannot be found.
		//
		inline const std::shared_ptr<Order>& removeOrder(std::int64_t instrumentId, std::int64_t orderId)
		{
			OrderCollection& collection = orderByInstrumentMap[instrumentId];

			auto it = std::remove_if(collection.begin(), collection.end(), [] (const std::shared_ptr<Order>& order) {
				return order->getOrderId() == orderId;
			});

			return it == collection.end() ? std::shared_ptr<Order> () : *it;
		}

	private:

		// The object pool for orders.
		static Pool<Order, 1000> pool;

		// the order map by instrument.
		std::unordered_map<std::int64_t, OrderCollection> orderByInstrumentMap;
	};
}


#endif /* LIB_ORDER_SRC_ORDERCACHE_HPP_ */
