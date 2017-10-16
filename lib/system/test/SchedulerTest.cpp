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

		std::this_thread::sleep_for(std::chrono::milliseconds(11));
		ASSERT_TRUE(done);
	}
}


