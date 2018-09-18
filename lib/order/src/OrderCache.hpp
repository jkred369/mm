/*
 * OrderCache.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDERCACHE_HPP_
#define LIB_ORDER_SRC_ORDERCACHE_HPP_

#include <unordered_map>

#include <Order.hpp>

namespace mm
{
	//
	// This class represents an order cache for all live orders in the market.
	//
	template<typename Order> class OrderCache
	{
	public:

		//
		// Determine if the order can potentially cross with another order.
		//
		// newOrder : The order to be sent.
		//
		bool hasCrossOrder(const Order& newOrder)
		{
			for (const std::pair<std::int64_t, const Order*> pair : orderByInstrumentMap[newOrder.instrumentId])
			{
				if ((pair.second->side == Side::BID && newOrder->side == Side::ASK && pair.second->price >= newOrder.price) ||
					(pair.second->side == Side::ASK && newOrder->side == Side::BID && pair.second->price <= newOrder.price))
				{
					return true;
				}
			}

			return false;
		}

	private:

		// Type of the order map.
		typedef std::unordered_map<std::int64_t, const Order*> OrderMap;

		// the order map by instrument.
		std::unordered_map<std::int64_t, OrderMap> orderByInstrumentMap;
	};
}


#endif /* LIB_ORDER_SRC_ORDERCACHE_HPP_ */
