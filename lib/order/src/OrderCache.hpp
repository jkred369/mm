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

#include <NativeDefinition.hpp>
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
		typedef std::vector<Order*> OrderCollection;

		//
		// Determine if the order can potentially cross with another order.
		//
		// newOrder : The order to be sent.
		//
		bool hasCrossOrder(const Order* newOrder) const
		{
			auto conIt = orderByInstrumentMap.find(newOrder->getInstrumentId());
			if (UNLIKELY(conIt == orderByInstrumentMap.end()))
			{
				return false;
			}

			for (const Order* order : conIt->second)
			{
				if ((order->getSide() == Side::BID && newOrder->getSide() == Side::ASK && order->getPrice() >= newOrder->getPrice()) ||
					(order->getSide() == Side::ASK && newOrder->getSide() == Side::BID && order->getPrice() <= newOrder->getPrice()))
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
		inline void addOrder(Order* newOrder)
		{
			OrderCollection& collection = orderByInstrumentMap[newOrder->getInstrumentId()];

			auto it = std::find_if(collection.begin(), collection.end(), [&] (const Order* order) {
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
		inline Order* getOrder(std::int64_t instrumentId, std::int64_t orderId) const
		{
			auto conIt = orderByInstrumentMap.find(instrumentId);
			if (UNLIKELY(conIt == orderByInstrumentMap.end()))
			{
				return nullptr;
			}

			const OrderCollection& collection = conIt->second;

			auto it = std::find_if(collection.begin(), collection.end(), [&] (const Order* order) {
				return order->getOrderId() == orderId;
			});

			return it == collection.end() ? nullptr : *it;
		}

		//
		// Remove an order from the order cache.
		//
		// instrumentId : The instrument ID.
		// orderId : The order ID.
		//
		// return : the removed order; or empty pointer if the order cannot be found.
		//
		inline Order* removeOrder(std::int64_t instrumentId, std::int64_t orderId)
		{
			OrderCollection& collection = orderByInstrumentMap[instrumentId];

			auto it = std::remove_if(collection.begin(), collection.end(), [&] (const Order* order) {
				return order->getOrderId() == orderId;
			});

			if (LIKELY(it != collection.end()))
			{
				Order* result = *it;
				collection.erase(it);
				return result;
			}

			return nullptr;
		}

	private:

		// Logger of the class.
		static Logger logger;

		// the order map by instrument.
		std::unordered_map<std::int64_t, OrderCollection> orderByInstrumentMap;
	};
}

template<typename Order> mm::Logger mm::OrderCache<Order>::logger;

#endif /* LIB_ORDER_SRC_ORDERCACHE_HPP_ */
