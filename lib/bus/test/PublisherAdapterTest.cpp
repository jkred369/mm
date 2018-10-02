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

		std::atomic<int> counter;
	};

	struct DummyConsumer : IConsumer<DummyMessage>
	{
		virtual void consume(const std::shared_ptr<const DummyMessage>& message) override
		{
			++counter;
		}

		std::atomic<int> counter;
	};

	TEST(PublisherAdapterTest, DummyCase)
	{
		KeyType dispatchKey = 5;
		Dispatcher dispatcher;

		PublisherAdapter<DummyMessage> adapter(dispatchKey, dispatcher);

		const Subscription subscription = {SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, 1};
		adapter.publish(subscription, std::make_shared<DummyMessage>());
	}

}



