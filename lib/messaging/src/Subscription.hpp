/*
 * Subscription.hpp
 *
 *  Created on: Mar 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_SUBSCRIPTION_HPP_
#define LIB_MESSAGING_SRC_SUBSCRIPTION_HPP_

#include <functional>

#include <ISubscription.hpp>
#include <SingletonInstance.hpp>

namespace mm
{
	//
	// The subscription class provides the default implementation for the subscription interface.
	//
	class Subscription : public BaseSubscription, public SingletonInstance<Subscription>
	{
	public:

		virtual ~Subscription()
		{
		}

		virtual SourceType getSourceType() const override
		{
			return sourceType;
		}

		virtual DataType getDataType() const override
		{
			return dataType;
		}

		virtual const std::int64_t& getKey() const override
		{
			return key;
		}

		//
		// Implement custom comparison for the subscription.
		//
		// rhs : The other subscription instance.
		//
		// return : true if the 2 subscription objects are identical.
		//
		bool operator == (const Subscription& rhs) const
		{
			return sourceType == rhs.sourceType &&
					dataType == rhs.dataType &&
					key == rhs.key;
		}

	private:

		// Source type
		SourceType sourceType;

		// Data type
		DataType dataType;

		// Key of the subscription
		std::int64_t key;
	};
}

namespace std
{
	template<> struct hash<mm::Subscription>
	{
		std::size_t operator()(const mm::Subscription& value) const
		{
			return std::hash<std::int16_t> (value.sourceType) ^
					std::hash<std::int16_t> (value.dataType) ^
					std::hash<std::int64_t> (value.key);
		}
	};
}

#endif /* LIB_MESSAGING_SRC_SUBSCRIPTION_HPP_ */
