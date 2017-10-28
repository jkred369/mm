#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include <CountDownLatch.hpp>

#include "Dispatcher.hpp"
#include "DispatchOnceWorker.hpp"

namespace mm
{
	TEST(DispatchOnceWorkerTest, DummyCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);

		std::int32_t id = 1;
		std::int32_t count = 0;

		Runnable task = [&count] () { ++count; };

		DispatchOnceWorker<HashDispatcher<std::int32_t, std::mutex> > worker(id, task, dispatcher);
		worker.dispatch();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_EQ(count, 1);
	}

	TEST(DispatchOnceWorkerTest, OnceCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);

		std::int32_t id = 1;
		std::int32_t count = 0;
		CountDownLatch<> latch(1);

		Runnable task = [&count, &latch] () { latch.wait(); ++count; };

		DispatchOnceWorker<HashDispatcher<std::int32_t, std::mutex> > worker(id, task, dispatcher);
		worker.dispatch();
		worker.dispatch();
		worker.dispatch();

		latch.countDown();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_EQ(count, 1);
	}

}
