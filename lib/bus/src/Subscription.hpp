/*
 * Subscription.hpp
 *
 *  Created on: Mar 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_SUBSCRIPTION_HPP_
#define LIB_BUS_SRC_SUBSCRIPTION_HPP_

#include <functional>
#include <ostream>

#include <EnumType.hpp>
#include <DataType.hpp>
#include <DispatchKey.hpp>
#include <SourceType.hpp>

namespace mm
{
	//
	// The subscription class provides the default implementation for the subscription interface.
	//
	template<typename Key = KeyType> struct SubscriptionT
	{
		//
		// constructor for singleton.
		//
		// sourceType : The source type.
		// dataType : The data type.
		// key : The key.
		//
		SubscriptionT(SourceType sourceType, DataType dataType, KeyType key) :
			sourceType(sourceType), dataType(dataType), key(key)
		{
		}

		//
		// Implement custom comparison for the subscription.
		//
		// rhs : The other subscription instance.
		//
		// return : true if the 2 subscription objects are identical.
		//
		bool operator == (const SubscriptionT& rhs) const
		{
			return sourceType == rhs.sourceType &&
					dataType == rhs.dataType &&
					key == rhs.key;
		}

		//
		// Implement custom comparison for the subscription.
		//
		// rhs : The other subscription instance.
		//
		// return : true if the 2 subscription objects are different.
		//
		bool operator != (const SubscriptionT& rhs) const
		{
			return !(operator == (rhs));
		}

		// Source type
		SourceType sourceType;

		// Data type
		DataType dataType;

		// Key of the subscription
		Key key;
	};

	// The ID for subscriting to all keys on this subscription
	static constexpr KeyType ALL_ID = -1;

	typedef SubscriptionT<KeyType> Subscription;
}

namespace std
{
	template<> struct hash<mm::Subscription>
	{
		std::size_t operator()(const mm::Subscription& value) const
		{
			return std::hash<std::int16_t>{} (static_cast<std::int16_t>(value.sourceType)) ^
					std::hash<std::int16_t>{} (static_cast<std::int16_t>(value.dataType)) ^
					std::hash<mm::KeyType>{} (value.key);
		}
	};
}

#endif /* LIB_MESSAGING_SRC_SUBSCRIPTION_HPP_ */
