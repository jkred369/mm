#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <boost/intrusive_ptr.hpp>

#include <gtest/gtest.h>

#include "Recyclable.hpp"
#include "LockFreeObjectPool.hpp"

namespace mm
{
	struct TestClass : public Recyclable<TestClass>
	{
		~TestClass()
		{
			counter->fetch_add(1);
		}

		std::atomic<std::int64_t>* counter;
	};

	TEST(RecyclableTest, ClassCase)
	{
		std::atomic<std::int64_t> counter(0);
		LockFreeObjectPool<TestClass> pool(2, false);
		ASSERT_TRUE(!pool.empty());

		TestClass* value1 = nullptr;
		TestClass* value2 = nullptr;
		{
			boost::intrusive_ptr<TestClass> value = pool.get();
			ASSERT_TRUE(value.get() != nullptr);
			ASSERT_TRUE(!pool.empty());

			value->counter = &counter;
			value1 = value.get();
		}

		ASSERT_TRUE(counter.load() == 1);

		{
			boost::intrusive_ptr<TestClass> value = pool.get();
			ASSERT_TRUE(value.get() != nullptr);
			ASSERT_TRUE(!pool.empty());

			value->counter = &counter;
			value2 = value.get();
		}

		ASSERT_TRUE(counter.load() == 2);
		ASSERT_TRUE(value1 == value2);
	}

}
