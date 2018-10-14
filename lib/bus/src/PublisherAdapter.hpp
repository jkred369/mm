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
		// dispatcher : The dispatcher used.
		//
		PublisherAdapter(Dispatcher& dispatcher) : dispatcher(dispatcher), count(0)
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
			bool published = publishInternal(subscription, message);

			// TODO: review the publish policy. should we determine the source type with the adapter?
			if (subscription.sourceType != SourceType::ALL && subscription.key != ALL_ID)
			{
				// publish for : ALL-dataType-key, souceType-dataType-ALL, ALL-dataType-ALL
				const Subscription allTypeSub = {SourceType::ALL, subscription.dataType, subscription.key};
				const Subscription allKeySub = {subscription.sourceType, subscription.dataType, ALL_ID};
				const Subscription allSub = {SourceType::ALL, subscription.dataType, ALL_ID};

				published |= publishInternal(allTypeSub, message);
				published |= publishInternal(allKeySub, message);
				published |= publishInternal(allSub, message);
			}
			else if (subscription.sourceType != SourceType::ALL || subscription.key != ALL_ID)
			{
				// in either case we only do for ALL-dataType-ALL
				const Subscription allSub = {SourceType::ALL, subscription.dataType, ALL_ID};
				published |= publishInternal(allSub, message);
			}

			if (UNLIKELY(!published))
			{
				LOGWARN("Cannot find consumers for subscription {}-{}-{}",
						toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
			}
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

			std::vector<ConsumerDetail>& consumers = consumerMap[subscription];
			std::size_t prevCount = consumers.size();

			auto it = std::remove_if(consumers.begin(), consumers.end(), [&consumer] (const ConsumerDetail& detail) {
				return detail.consumer == consumer;
			});

			if (LIKELY(it != consumers.end()))
			{
				consumers.erase(it);
				count -= prevCount - consumers.size();
			}
		}

		virtual std::size_t getConsumerCount() const override
		{
			return count;
		}

		virtual std::size_t getConsumerCount(const Subscription& subscription) const override
		{
			auto it = consumerMap.find(subscription);
			return it == consumerMap.end() ? 0 : it->second.size();
		}

		virtual void removeAll() override
		{
			consumerMap.clear();
			count = 0;
		}

	protected:

		//
		// Publish message to the given subscription.
		//
		// subscription : The subscription.
		// message : The message.
		//
		// return : true if the subscription is found.
		//
		inline bool publishInternal(const Subscription& subscription, const std::shared_ptr<const Message>& message)
		{
			auto it = consumerMap.find(subscription);
			if (LIKELY(it != consumerMap.end()))
			{
				for (const ConsumerDetail& detail : it->second)
				{
					publishTo(detail.dispatchKey, detail.consumer, message);
				}

				return true;
			}

			return false;
		}

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
