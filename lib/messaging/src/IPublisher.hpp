/*
 * IPublisher.hpp
 *
 *  Created on: Mar 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_IPUBLISHER_HPP_
#define LIB_BUS_SRC_IPUBLISHER_HPP_

#include <memory>

#include <IConsumer.hpp>
#include <ISubscription.hpp>
#include <Message.hpp>

namespace mm
{
	//
	// The publisher defines the interface for sending a message.
	//
	class IPublisher
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
		virtual void publish(const ISubscription& subscription, const std::shared_ptr<Message> message) = 0;

		//
		// Add the consumer to the publisher.
		//
		// consumer : The consumer.
		//
		virtual void addConsumer(IConsumer& consumer) = 0;

		//
		// Remove the given consumer from the publisher.
		//
		// consumer : The consumer to be removed.
		//
		virtual void removeConsumer(IConsumer& consumer) = 0;

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
