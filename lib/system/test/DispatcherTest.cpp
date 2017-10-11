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
}
