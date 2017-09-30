#include <chrono>
#include <thread>

#include <gtest/gtest.h>

#include "Condition.hpp"

namespace mm
{
	TEST(ConditionTest, DummyCase)
	{
		mm::Condition<> condition;
		bool done = false;

		std::thread t1([&condition, &done] ()
		{
			condition.wait();
			done = true;
		});

		std::thread t2([condition] ()
		{
			condition.notifyOne();
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		ASSERT_TRUE(done);
	}
}
