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
#include <SpinLockGuard.hpp>
#include <Subscription.hpp>

namespace mm
{
	//
	// This class provides basic implementation for publisher.
	//
	template<typename Message, typename Mutex = std::mutex> class PublisherAdapter : public IPublisher<Message>
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

		virtual void publish(const Subscription& subscription, const std::shared_ptr<const Message>& message) override
		{
			SpinLockGuard<Mutex> guard(mutex);

			auto it = consumerMap.find(subscription);
			if (it == consumerMap.end())
			{
				LOGWARN("Cannot find consumers for subscription {}-{}-{}", toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
				return;
			}

			for (const ConsumerDetail& detail : it->second)
			{
				// TODO: this piece of code needs to be reviewed for performance.
				IConsumer<Message>* consumer = detail.consumer;

				dispatcher.submit(detail.dispatchKey, [consumer, message] () {
					consumer->consume(message);
				});
			}
		}

		virtual bool subscribe(const Subscription& subscription, IConsumer<Message>* consumer) override
		{
			// safe guard for input
			if (consumer == nullptr)
			{
				LOGERR("Ignoring null consumer for subscription {}-{}-{}", toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
				return false;
			}

			SpinLockGuard<Mutex> guard(mutex);
			std::vector<ConsumerDetail>& consumers = consumerMap[subscription];

			// sanity check for duplicated subscription
			if (std::find_if(consumers.begin(), consumers.end(), [&consumer] (const ConsumerDetail& detail) {
				return detail.consumer == consumer;
			}) != consumers.end())
			{
				LOGWARN("Consumer on {} already submitted to {}-{}-{}. Ignoring.", consumer->getKey(),
						toValue(subscription.sourceType), toValue(subscription.dataType), subscription.key);
				return true;
			}

			consumerMap[subscription].push_back(ConsumerDetail(consumer));
			++count;

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

			SpinLockGuard<Mutex> guard(mutex);
			std::vector<ConsumerDetail>& consumers = consumerMap[subscription];

			consumers.erase(std::remove_if(consumers.begin(), consumers.end(), [&consumer] (const ConsumerDetail& detail) {
				return detail.consumer == consumer;
			}));
		}

		virtual size_t getConsumerCount() const override
		{
			SpinLockGuard<Mutex> guard(mutex);
			return count;
		}

		virtual void removeAll() override
		{
			SpinLockGuard<Mutex> guard(mutex);
			consumerMap.clear();
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

		// The mutex used to access the consumer map.
		mutable Mutex mutex;
	};
}

template<typename Message, typename Mutex> mm::Logger mm::PublisherAdapter<Message, Mutex>::logger;


#endif /* LIB_BUS_SRC_PUBLISHERADAPTER_HPP_ */
