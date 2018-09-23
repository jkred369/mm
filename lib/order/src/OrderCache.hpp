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

#include <NativeDefintion.hpp>
#include <Order.hpp>

namespace mm
{
	//
	// This class represents an order cache for all live orders in the market.
	//
	template<typename Order> class OrderCache
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
		// Add order to the cache.
		//
		// order : The order to add.
		//
		inline void addOrder(const shared_ptr<Order>& newOrder)
		{
			OrderCollection& collection = orderByInstrumentMap[newOrder->getInstrumentId()];

			auto it = std::find_if(collection.begin(), collection.end(), [] (const std::shared_ptr<Order>& order) {
				return order->getOrderId() == newOrder->getOrderId();
			});

			if (UNLIKELY(it != collection.end()))
			{
				LOGERR("Error adding order: order with same key already exists: {}", newOrder->getOrderId());
				return;
			}

			collection.push_back(newOrder);
		}

		//
		// Get an order from the order cache.
		//
		// instrumentId : The instrument ID.
		// orderId : The order ID.
		//
		// return : the pointer for the order; or empty pointer if order cannot be found.
		//
		inline const std::shared_ptr<Order>& getOrder(std::int64_t instrumentId, std::int64_t orderId)
		{
			OrderCollection& collection = orderByInstrumentMap[instrumentId];

			auto it = std::find_if(collection.begin(), collection.end(), [] (const std::shared_ptr<Order>& order) {
				return order->getOrderId() == orderId;
			});

			return it == collection.end() ? std::shared_ptr<Order> () : *it;
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

		// the order map by instrument.
		std::unordered_map<std::int64_t, OrderCollection> orderByInstrumentMap;
	};
}


#endif /* LIB_ORDER_SRC_ORDERCACHE_HPP_ */
