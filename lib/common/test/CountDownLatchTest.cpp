#include <chrono>
#include <thread>
#include <iostream>

#include <gtest/gtest.h>

#include "../src/CountDownLatch.hpp"

namespace mm
{
	TEST(CountDownLatchTest, ThreadOrderingCase)
	{
		mm::CountDownLatch<> latch(1);
		bool done = false;

		std::thread t1([&latch, &done] ()
		{
			latch.wait();
			done = true;
		});

		std::thread t2([&latch] ()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			latch.countDown();
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		ASSERT_TRUE(done);

		t1.join();
		t2.join();
	}
}
