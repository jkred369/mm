/*
 * ISubscriber.hpp
 *
 *  Created on: Sep 12, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_ISUBSCRIBER_HPP_
#define LIB_BUS_SRC_ISUBSCRIBER_HPP_

#include <memory>

#include <IConsumer.hpp>
#include <ISubscription.hpp>
#include <Message.hpp>

namespace mm
{
	//
	// The subscriber defines the interface for subscribing to a message.
	//
	template<typename Message> class ISubscriber
	{
	public:

		virtual ~ISubscriber()
		{
		}

		//
		// Subscribe to a message.
		//
		// subscription : The topic on which the message will be received.
		//
		virtual void subscribe(const ISubscription& subscription) = 0;

		//
		// Add the consumer to the subscriber.
		//
		// consumer : The consumer.
		//
		virtual void addConsumer(IConsumer<Message>& consumer) = 0;

		//
		// Remove the given consumer from the subscriber.
		//
		// consumer : The consumer to be removed.
		//
		virtual void removeConsumer(IConsumer<Message>& consumer) = 0;

		//
		// Get the count of consumer.
		//
		// return : The count of the consumers.
		//
		virtual size_t getConsumerCount() const = 0;

		//
		// Remove all the consumers.
		//
		virtual void removeAll() = 0;
	};
}

#endif /* LIB_BUS_SRC_ISUBSCRIBER_HPP_ */
