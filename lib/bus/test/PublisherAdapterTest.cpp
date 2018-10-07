/*
 * PublisherAdapterTest.cpp
 *
 *  Created on: Sep 26, 2018
 *      Author: suoalex
 */

#include <atomic>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include <Dispatcher.hpp>
#include <Subscription.hpp>

#include "PublisherAdapter.hpp"

namespace mm
{
	struct DummyMessage
	{
		DummyMessage() : counter(0)
		{
		}

		DummyMessage(const DummyMessage& message)
		{
			counter.store(message.counter.load());
		}

		std::atomic<int> counter;
	};

	struct DummyPublisherAdapter : PublisherAdapter<DummyMessage>
	{
		DummyPublisherAdapter(Dispatcher& dispatcher, QueueBasedObjectPool<DummyMessage>& pool) : PublisherAdapter<DummyMessage>(dispatcher, pool)
		{
		}

		virtual const KeyType getKey() const override
		{
			return 1;
		}
	};

	struct DummyConsumer : IConsumer<DummyMessage>
	{
		virtual void consume(const DummyMessage* message) override
		{
			++counter;
		}

		std::atomic<int> counter;
	};

	TEST(PublisherAdapterTest, DummyCase)
	{
		Dispatcher dispatcher;
		QueueBasedObjectPool<DummyMessage> pool(10);

		DummyPublisherAdapter adapter(dispatcher, pool);

		const Subscription subscription = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, 1};

		DummyMessage message;
		adapter.publish(subscription, &message);
	}

}



