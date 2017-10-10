#include <cstdint>
#include <chrono>
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

		std::shared_ptr<Runnable> task([&done] () {
			done = true;
		});

		dispatcher.submit(id, task);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_TRUE(done);
	}
}
