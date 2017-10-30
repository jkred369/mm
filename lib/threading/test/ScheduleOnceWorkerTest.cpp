#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include <CountDownLatch.hpp>
#include <TimeUtil.hpp>

#include "Dispatcher.hpp"
#include "Scheduler.hpp"
#include "ScheduleOnceWorker.hpp"

namespace mm
{
	TEST(ScheduleOnceWorkerTest, DummyCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);
		DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > scheduler(dispatcher);

		std::int32_t id = 1;
		std::int32_t count = 0;

		Runnable task = [&count] () { ++count; };

		std::int64_t delay = durationToNanos(std::chrono::milliseconds(1));
		ScheduleOnceWorker<DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > > worker(
				id, task, delay, scheduler);
		worker.schedule();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_EQ(count, 1);
	}

	TEST(ScheduleOnceWorkerTest, OnceCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);
		DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > scheduler(dispatcher);

		std::int32_t id = 1;
		std::int32_t count = 0;
		CountDownLatch<> latch(1);

		Runnable task = [&count, &latch] () { latch.wait(); ++count; };

		std::int64_t delay = durationToNanos(std::chrono::milliseconds(1));
		ScheduleOnceWorker<DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > > worker(
				id, task, delay, scheduler);
		worker.schedule();
		worker.schedule();
		worker.schedule();

		latch.countDown();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_EQ(count, 1);
	}

	TEST(ScheduleOnceWorkerTest, TwiceCase)
	{
		HashDispatcher<std::int32_t, std::mutex> dispatcher(2);
		DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > scheduler(dispatcher);

		std::int32_t id = 1;
		std::int32_t count = 0;
		CountDownLatch<> latch(1);

		Runnable task = [&count, &latch] () { ++count; latch.wait(); };

		std::int64_t delay = durationToNanos(std::chrono::milliseconds(1));
		ScheduleOnceWorker<DefaultScheduler<HashDispatcher<std::int32_t, std::mutex> > > worker(
				id, task, delay, scheduler);
		worker.schedule();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		latch.countDown();

		worker.schedule();
		worker.schedule();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ASSERT_EQ(count, 2);
	}

}
