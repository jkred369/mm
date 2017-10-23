#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include "Dispatcher.hpp"
#include "Scheduler.hpp"

namespace mm
{
	TEST(SchedulerTest, DummyCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);
		DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > scheduler(dispatcher);

		std::int32_t id = 1;
		bool done = false;

		scheduler.schedule(id, [&done] () { done = true; }, std::chrono::milliseconds(10));

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		ASSERT_TRUE(done);
	}

	TEST(SchedulerTest, JumpQueueCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);
		DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > scheduler(dispatcher);

		// a later task comes first.
		bool done1 = false;
		scheduler.schedule(1, [&done1] () { done1 = true; }, std::chrono::milliseconds(2000));

		// sleep to ensure the first one running.
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// an earlier task comes later.
		bool done2 = false;
		scheduler.schedule(2, [&done2] () { done2 = true; }, std::chrono::milliseconds(10));

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		ASSERT_TRUE(!done1);
		ASSERT_TRUE(done2);
	}

}

