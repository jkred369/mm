/*
 * PublisherAdapter.hpp
 *
 *  Created on: Sep 22, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_PUBLISHERADAPTER_HPP_
#define LIB_BUS_SRC_PUBLISHERADAPTER_HPP_

#include <algorithm>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <Dispatcher.hpp>
#include <IPublisher.hpp>
#include <Logger.hpp>
#include <NativeDefinition.hpp>
#include <SpinLockGuard.hpp>
#include <Subscription.hpp>

namespace mm
{
	//
	// This class provides basic implementation for publisher.
	//
	template<typename Message> class PublisherAdapter : public IPublisher<Message>
	{
	public:

		//
		// Constructor.
		//
		// dispatchKey : The dispatch key.
		// dispatcher : The dispatcher used.
		//
		PublisherAdapter(KeyType dispatchKey, Dispatcher& dispatcher) : dispatchKey(dispatchKey), dispatcher(dispatcher), count(0)
		{
		}

		virtual ~PublisherAdapter()
		{
		}

		//
		// By default we don't do any snapshot.
		//
		virtual std::size_t initSnapshot(IConsumer<Message>* consmer) const override
		{
			return 0;
		}

		virtual void publish(const Subscription& subscription, const std::shared_ptr<const Message>& message) override
		{
			dispatcher.submit(dispatchKey, [subscription, message, this] () {
				auto it = consumerMap.find(subscription);
				if (UNLIKELY(it == consumerMap.end()))
				{
					LOGWARN("Cannot find consumers for subscription {}-{}-{}",
							toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
					return;
				}

				for (const ConsumerDetail& detail : it->second)
				{
					publishTo(detail.dispatchKey, detail.consumer, message);
				}
			});
		}

		virtual bool subscribe(const Subscription& subscription, IConsumer<Message>* consumer) override
		{
			// safe guard for input
			if (consumer == nullptr)
			{
				LOGERR("Ignoring null consumer for subscription {}-{}-{}",
						toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
				return false;
			}

			dispatcher.submit(dispatchKey, [subscription, consumer, this] () {
				std::vector<ConsumerDetail>& consumers = consumerMap[subscription];

				// sanity check for duplicated subscription
				if (std::find_if(consumers.begin(), consumers.end(), [&consumer] (const ConsumerDetail& detail) {
					return detail.consumer == consumer;
				}) == consumers.end())
				{
					consumerMap[subscription].push_back(ConsumerDetail(consumer));
					++count;
				}
				else
				{
					LOGWARN("Consumer on {} already submitted to {}-{}-{}. Ignoring.", consumer->getKey(),
							toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
				}
			});

			return true;
		}

		virtual void unsubscribe(const Subscription& subscription, IConsumer<Message>* consumer) override
		{
			// safe guard for input
			if (consumer == nullptr)
			{
				LOGERR("Ignoring null consumer for unsubscribing {}-{}-{}", toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
				return;
			}

			dispatcher.submit(dispatchKey, [subscription, consumer, this] () {
				std::vector<ConsumerDetail>& consumers = consumerMap[subscription];

				consumers.erase(std::remove_if(consumers.begin(), consumers.end(), [&consumer] (const ConsumerDetail& detail) {
					return detail.consumer == consumer;
				}));
			});
		}

		virtual void removeAll() override
		{
			dispatcher.submit(dispatchKey, [&] () {
				consumerMap.clear();
			});
		}

	protected:

		//
		// Utility to publish to a specific consumer (without flashing the existing consumers).
		//
		// dispatchKey : The dispatch key.
		// consumer : The consumer.
		// message : The message to publish.
		//
		inline void publishTo(KeyType dispatchKey, IConsumer<Message>* consumer, const std::shared_ptr<const Message>& message)
		{
			// TODO: this piece of code needs to be reviewed for performance.
			dispatcher.submit(dispatchKey, [consumer, message] () {
				consumer->consume(message);
			});
		}

	private:

		//
		// The struct holding consumer details.
		//
		struct ConsumerDetail
		{
			//
			// Constructor.
			//
			// consuemr : The consumer detail captured.
			//
			ConsumerDetail(IConsumer<Message>* consumer) :
				dispatchKey(consumer->getKey()), consumer(consumer)
			{
			}

			// The cached dispatch key.
			KeyType dispatchKey;

			// The consumer.
			IConsumer<Message>* consumer;
		};

		// The logger for this class.
		static Logger logger;

		// The dispatch key.
		const KeyType dispatchKey;

		// The dispatcher.
		Dispatcher& dispatcher;

		// The map where key is the subscription object and value is the consumer(s) subscribing to it.
		std::unordered_map<Subscription, std::vector<ConsumerDetail> > consumerMap;

		// The total number of consumers.
		std::size_t count;
	};
}

template<typename Message> mm::Logger mm::PublisherAdapter<Message>::logger;


#endif /* LIB_BUS_SRC_PUBLISHERADAPTER_HPP_ */
