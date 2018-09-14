/*
 * DefaultSubscriber.hpp
 *
 *  Created on: Sep 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_SUBSCRIBERADAPTER_HPP_
#define LIB_MESSAGING_SRC_SUBSCRIBERADAPTER_HPP_

#include <mutex>

#include <ISubscriber.hpp>

namespace mm
{
	//
	// The class provides a default implementation of the ISubscriber interface.
	//
	template<typename Message> class SubscriberAdapter : public ISubscriber<Message>
	{
	public:

		virtual ~SubscriberAdapter()
		{
		}

		virtual void subscribe(const ISubscription& subscription) override
		{
		}

		virtual void addConsumer(IConsumer<Message>& consumer) override
		{
			std::unique_lock<std::mutex> lock(mutex);
			consumers.push_back(&consumer);
		}

		virtual void removeConsumer(IConsumer<Message>& consumer) override
		{
			std::unique_lock<std::mutex> lock(mutex);
			std::remove(consumers.begin(), consumers.end(), &consumer);
		}

		virtual size_t getConsumerCount() const override
		{
			std::unique_lock<std::mutex> lock(mutex);
			return consumers.size();
		}

		virtual void removeAll() override
		{
			std::unique_lock<std::mutex> lock(mutex);
			consumers.clear();
		}

	protected:

		//
		// fire the even to the consumers.
		//
		// message : The message send to consumers.
		//
		inline void notify(const std::shared_ptr<const Message>& message)
		{
			for (IConsumer<Message>* consumer : consumers)
			{
				consumer->consume(message);
			}
		}

	private:

		// The consumers.
		std::vector<IConsumer<Message>*> consumers;

		// The mutex for accessing the consumer.
		std::mutex mutex;
	};
}

#endif /* LIB_MESSAGING_SRC_SUBSCRIBERADAPTER_HPP_ */
