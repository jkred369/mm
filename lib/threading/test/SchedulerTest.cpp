#include "../../threading/src/Scheduler.hpp"

#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include "../../threading/src/Dispatcher.hpp"

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
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// an earlier task comes later.
		bool done2 = false;
		scheduler.schedule(2, [&done2] () { done2 = true; }, std::chrono::milliseconds(10));

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		ASSERT_TRUE(!done1);
		ASSERT_TRUE(done2);
	}

	TEST(SchedulerTest, MultipleJumpQueueCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);
		DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > scheduler(dispatcher);

		// a later task comes first.
		bool done1 = false;
		scheduler.schedule(1, [&done1] () { done1 = true; }, std::chrono::milliseconds(2000));

		// sleep to ensure the first one running.
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// an earlier task comes later.
		bool done2 = false;
		scheduler.schedule(2, [&done2] () { done2 = true; }, std::chrono::milliseconds(10));

		// sleep to ensure the first one running.
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		// an earlier task comes later.
		bool done3 = false;
		scheduler.schedule(2, [&done3] () { done3 = true; }, std::chrono::milliseconds(10));

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		ASSERT_TRUE(!done1);
		ASSERT_TRUE(done2);
		ASSERT_TRUE(done3);
	}

	TEST(SchedulerTest, ScheduleAtFixedRateCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);
		DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > scheduler(dispatcher);

		std::int32_t id = 1;
		std::int32_t count = 0;

		scheduler.scheduleAtFixedRate(id, [&count] () { ++count; },
				std::chrono::milliseconds(1),
				std::chrono::milliseconds(15));

		std::this_thread::sleep_for(std::chrono::milliseconds(40));
		ASSERT_EQ(count, 3);
	}

}


