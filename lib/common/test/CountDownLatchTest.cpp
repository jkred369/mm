#include <chrono>
#include <thread>
#include <iostream>

#include <gtest/gtest.h>

#include "../src/CountDownLatch.hpp"

namespace mm
{
	TEST(CountDownLatchTest, DummyCase)
	{
		mm::CountDownLatch<> latch(0);
		bool done = false;

		std::thread t1([&latch, &done] ()
		{
			latch.wait();
			done = true;
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_TRUE(done);

		t1.join();
	}

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
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			latch.countDown();
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		ASSERT_TRUE(done);

		t1.join();
		t2.join();
	}

	TEST(CountDownLatchTest, MultiThreadCase)
	{
		mm::CountDownLatch<> latch(2);
		bool done = false;

		std::thread t1([&latch, &done] ()
		{
			latch.wait();
			done = true;
		});

		std::thread t2([&latch] ()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			latch.countDown();
		});

		std::thread t3([&latch] ()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			latch.countDown();
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		ASSERT_TRUE(done);

		t1.join();
		t2.join();
		t3.join();
	}
}
