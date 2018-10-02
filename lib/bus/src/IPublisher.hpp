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
#include <IDispatchable.hpp>
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
		// Request an initial snapshot from the publisher before the whole context starts.
		//
		// This can be seen as a violation of the pub/sub structure but sometimes such a snapshot is needed,
		// especially for the (almost) static data stores. The snapshot is done before dispatcher start up,
		// most likely during construction. Thus calling of the consumer is still guaranteed to be thread safe.
		//
		// consumer : The consumer.
		//
		// return : no. of messages passed to consumer.
		//
		virtual std::size_t initSnapshot(IConsumer<Message>* consumer) const = 0;

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
		// return : true if the subscription is successful.
		//
		virtual bool subscribe(const Subscription& subscription, IConsumer<Message>* consumer) = 0;

		//
		// Unsubscribe to a message.
		//
		// subscription : The topic to unsubscribe.
		// consumer : The consumer unsubscribing from the topic.
		//
		virtual void unsubscribe(const Subscription& subscription, IConsumer<Message>* consumer) = 0;

		//
		// Get consumer count.
		//
		// return : The total number of consumers.
		//
		virtual std::size_t getConsumerCount() const = 0;

		//
		// Get consumer count for a specific subscription.
		//
		// return : The total number for the given subscription.
		//
		virtual std::size_t getConsumerCount(const Subscription& subscription) const = 0;

		//
		// Remove all the consumers.
		//
		virtual void removeAll() = 0;
	};
}


#endif /* LIB_BUS_SRC_IPUBLISHER_HPP_ */
