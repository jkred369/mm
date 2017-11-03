#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include "ObjectPool.hpp"

namespace mm
{
	TEST(ObjectPoolTest, DummyCase)
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

}
