#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <boost/intrusive_ptr.hpp>

#include <gtest/gtest.h>

#include "ObjectPool.hpp"

namespace mm
{
	TEST(ObjectPoolTest, PrimitiveCase)
	{
		ObjectPool<std::int64_t> pool(1, false);
		ASSERT_TRUE(!pool.empty());

		{
			std::int64_t* value = pool.get();
			ASSERT_TRUE(value != nullptr);
			*value = 1;

			ASSERT_TRUE(pool.empty());
			ASSERT_TRUE(pool.get() == nullptr);

			pool.release(value);
			ASSERT_TRUE(!pool.empty());
		}

		{
			std::int64_t* value = pool.get();
			ASSERT_TRUE(value != nullptr);
			ASSERT_TRUE(*value == 0);

			ASSERT_TRUE(pool.empty());
			ASSERT_TRUE(pool.get() == nullptr);

			pool.release(value);
			ASSERT_TRUE(!pool.empty());
		}
	}

	TEST(ObjectPoolTest, ClassCase)
	{
		struct TestClass : public ObjectPool<TestClass>::Recyclable
		{
			~TestClass()
			{
				counter->fetch_sub(1);
			}

			std::atomic<std::int64_t>* counter;
		};

		std::atomic<std::int64_t> counter;
		ObjectPool<TestClass> pool(2, false);
		ASSERT_TRUE(!pool.empty());

		TestClass* value1 = nullptr;
		TestClass* value2 = nullptr;
		{
			boost::intrusive_ptr<TestClass> value = pool.get();
			ASSERT_TRUE(value != nullptr);
			ASSERT_TRUE(!pool.empty());

			value->counter = &counter;
			value1 = value.get();
		}

		ASSERT_TRUE(counter.load() == 1);

		{
			boost::intrusive_ptr<TestClass> value = pool.get();
			ASSERT_TRUE(value != nullptr);
			ASSERT_TRUE(!pool.empty());

			value->counter = &counter;
			value2 = value.get();
		}

		ASSERT_TRUE(counter.load() == 2);
		ASSERT_TRUE(value1 == value2);
	}

}
