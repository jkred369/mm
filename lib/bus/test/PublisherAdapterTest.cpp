/*
 * PublisherAdapterTest.cpp
 *
 *  Created on: Sep 26, 2018
 *      Author: suoalex
 */

#include <atomic>
#include <chrono>
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

		std::atomic<int> counter;
	};

	struct DummyPublisherAdapter : PublisherAdapter<DummyMessage>
	{
		DummyPublisherAdapter(Dispatcher& dispatcher) : PublisherAdapter<DummyMessage>(dispatcher)
		{
		}

		virtual const KeyType getKey() const override
		{
			return 1;
		}
	};

	struct DummyConsumer : IConsumer<DummyMessage>
	{
		DummyConsumer() : counter(0)
		{
		}

		virtual void consume(const std::shared_ptr<const DummyMessage>& message) override
		{
			++counter;
		}

		virtual const KeyType getKey() const
		{
			return DispatchKey::ALGO;
		}

		std::atomic<int> counter;
	};

	TEST(PublisherAdapterTest, DummyCase)
	{
		Dispatcher dispatcher;

		DummyPublisherAdapter adapter(dispatcher);

		const Subscription subscription = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, 1};
		adapter.publish(subscription, std::make_shared<DummyMessage>());
	}

	TEST(PublisherAdapterTest, SimpleCase)
	{
		Dispatcher dispatcher;

		DummyConsumer consumer;
		DummyPublisherAdapter adapter(dispatcher);

		const Subscription subscription = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, 1};
		adapter.subscribe(subscription, &consumer);

		adapter.publish(subscription, std::make_shared<DummyMessage>());
		std::this_thread::sleep_for(std::chrono::milliseconds(4));
		ASSERT_TRUE(consumer.counter.load() == 1);
	}

	TEST(PublisherAdapterTest, MultipleCase)
	{
		Dispatcher dispatcher;

		DummyConsumer consumer1;
		DummyConsumer consumer2;
		DummyPublisherAdapter adapter(dispatcher);

		const Subscription subscription = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, 1};
		adapter.subscribe(subscription, &consumer1);
		adapter.subscribe(subscription, &consumer2);

		adapter.publish(subscription, std::make_shared<DummyMessage>());
		std::this_thread::sleep_for(std::chrono::milliseconds(4));
		ASSERT_TRUE(consumer1.counter.load() == 1);
		ASSERT_TRUE(consumer2.counter.load() == 1);
	}

	TEST(PublisherAdapterTest, WildCardCase)
	{
		Dispatcher dispatcher;

		DummyConsumer consumer1;
		DummyConsumer consumer2;
		DummyConsumer consumer3;
		DummyConsumer consumer4;
		DummyPublisherAdapter adapter(dispatcher);

		const Subscription subscription = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, 1};
		adapter.subscribe(subscription, &consumer1);

		const Subscription allSource = {SourceType::ALL, DataType::MARKET_DATA, 1};
		adapter.subscribe(allSource, &consumer2);

		const Subscription allId = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, ALL_ID};
		adapter.subscribe(allId, &consumer3);

		const Subscription all = {SourceType::ALL, DataType::MARKET_DATA, ALL_ID};
		adapter.subscribe(all, &consumer4);

		adapter.publish(subscription, std::make_shared<DummyMessage>());
		std::this_thread::sleep_for(std::chrono::milliseconds(4));
		ASSERT_TRUE(consumer1.counter.load() == 1);
		ASSERT_TRUE(consumer2.counter.load() == 1);
		ASSERT_TRUE(consumer3.counter.load() == 1);
		ASSERT_TRUE(consumer4.counter.load() == 1);
	}

}



