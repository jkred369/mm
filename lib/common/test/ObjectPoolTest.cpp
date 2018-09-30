#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include <boost/intrusive_ptr.hpp>

#include <gtest/gtest.h>

#include "ObjectPool.hpp"
#include "Timer.hpp"

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

	TEST(ObjectPoolTest, MultipleCase)
	{
		ObjectPool<std::int64_t> pool(4, false);
		ASSERT_TRUE(!pool.empty());

		std::int64_t* value1 = nullptr;
		std::int64_t* value2 = nullptr;

		{
			std::int64_t* value = pool.get();
			ASSERT_TRUE(value != nullptr);
			value1 = value;
			*value = 1;

			ASSERT_TRUE(!pool.empty());
			pool.release(value);
		}

		{
			std::int64_t* value = pool.get();
			ASSERT_TRUE(value != nullptr);
			ASSERT_TRUE(*value == 0);

			value2 = value;

			ASSERT_TRUE(!pool.empty());
			pool.release(value);
		}

		ASSERT_TRUE(value1 == value2);

		// take all out then return
		{
			std::int64_t* values[4];

			for (int i = 0; i < 4; ++i)
			{
				values[i] = pool.get();
				ASSERT_TRUE(values[i] != nullptr);
			}

			ASSERT_TRUE(pool.empty());
			ASSERT_TRUE(pool.get() == nullptr);

			for (int i = 0; i < 4; ++i)
			{
				for (int j = i + 1; j < 4; ++j)
				{
					ASSERT_TRUE(values[i] != values[j]);
				}
			}

			// release all then re-take
			for (int i = 0; i < 4; ++i)
			{
				pool.release(values[i]);
			}

			for (int i = 0; i < 4; ++i)
			{
				values[i] = pool.get();
				ASSERT_TRUE(values[i] != nullptr);
			}

			ASSERT_TRUE(pool.empty());
			ASSERT_TRUE(pool.get() == nullptr);

			for (int i = 0; i < 4; ++i)
			{
				for (int j = i + 1; j < 4; ++j)
				{
					ASSERT_TRUE(values[i] != values[j]);
				}
			}
		}
	}

	TEST(ObjectPoolTest, SharedPtrCase)
	{
		ObjectPool<std::int64_t> pool(1, false);
		ASSERT_TRUE(!pool.empty());

		{
			{
				std::shared_ptr<std::int64_t> value = pool.getShared();
				ASSERT_TRUE(value.get());
				*value = 1;

				ASSERT_TRUE(pool.empty());
				ASSERT_TRUE(pool.getShared().get() == nullptr);
			}

			ASSERT_TRUE(!pool.empty());
		}

		{
			{
				std::shared_ptr<std::int64_t> value = pool.getShared();
				ASSERT_TRUE(value.get());
				ASSERT_TRUE(*value == 0);

				ASSERT_TRUE(pool.empty());
				ASSERT_TRUE(pool.getShared().get() == nullptr);
			}

			ASSERT_TRUE(!pool.empty());
		}
	}

	TEST(ObjectPoolTest, PerformanceCase)
	{
		ObjectPool<double> pool(10, false);
		ASSERT_TRUE(!pool.empty());

		HighResTimer timer;

		// naive case
		{
			const int count = 10000;
			double timeSum = 0;

			for (int i = 0; i < 10000; ++i)
			{
				const long start = timer.getTimeInNanos();
				double* value = new double();
				const long end = timer.getTimeInNanos();

				timeSum += end - start;
				delete value;
			}

			std::cout << "Native Time per retrieval: " << (timeSum / count) << std::endl;
		}

		// native pointer case
		{
			const int count = 10000;
			double timeSum = 0;

			for (int i = 0; i < 10000; ++i)
			{
				const long start = timer.getTimeInNanos();
				double* value = pool.get();
				const long end = timer.getTimeInNanos();

				timeSum += end - start;
				pool.release(value);
			}

			std::cout << "ptr Time per retrieval: " << (timeSum / count) << std::endl;
		}

		// shared ptr case
		{
			const int count = 10000;
			double timeSum = 0;

			for (int i = 0; i < 10000; ++i)
			{
				const long start = timer.getTimeInNanos();
				std::shared_ptr<double> value = pool.getShared();
				const long end = timer.getTimeInNanos();

				timeSum += end - start;
			}

			std::cout << "std::shared_ptr Time per retrieval: " << (timeSum / count) << std::endl;
		}

	}

}
