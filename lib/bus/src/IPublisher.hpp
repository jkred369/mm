/*
 * IPublisher.hpp
 *
 *  Created on: Mar 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_IPUBLISHER_HPP_
#define LIB_BUS_SRC_IPUBLISHER_HPP_

#include <memory>

#include <DispatchKey.hpp>
#include <IConsumer.hpp>
#include <Subscription.hpp>

namespace mm
{
	//
	// The publisher defines the interface for sending a message.
	//
	template<typename Message> class IPublisher
	{
	public:

		virtual ~IPublisher()
		{
		}

		//
		// Publish a message to the bus.
		//
		// subscription : The topic on which the message will be delivered.
		// message : Shared pointer to the message to be published.
		//
		virtual void publish(const Subscription& subscription, const std::shared_ptr<const Message>& message) = 0;

		//
		// Subscribe to a message.
		//
		// subscription : The topic on which the message will be received.
		// consumer : The consumer subscribing.
		//
		virtual void subscribe(const Subscription& subscription, IConsumer<Message>* consumer) = 0;

		//
		// Unsubscribe to a message.
		//
		// subscription : The topic to unsubscribe.
		// consumer : The consumer unsubscribing from the topic.
		//
		virtual void unsubscribe(const Subscription& subscription, IConsumer<Message>* consumer) = 0;

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


#endif /* LIB_BUS_SRC_IPUBLISHER_HPP_ */
