#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include "Dispatcher.hpp"

namespace mm
{
	TEST(DispatcherTest, DummyCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);

		std::int32_t id = 1;
		bool done = false;

		dispatcher.submit(id, [&done] () { done = true; });

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_TRUE(done);
	}

	TEST(DispatcherTest, MultiTaskCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);

		std::int32_t id1 = 1;
		bool done1 = false;
		dispatcher.submit(id1, [&done1] () { done1 = true; });

		std::int32_t id2 = 1;
		bool done2 = false;
		dispatcher.submit(id2, [&done2] () { done2 = true; });

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_TRUE(done1);
		ASSERT_TRUE(done2);
	}
}
