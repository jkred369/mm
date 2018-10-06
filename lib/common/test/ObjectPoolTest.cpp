#include <array>
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

	TEST(ObjectPoolTest, ConstructorCase)
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

	TEST(ObjectPoolTest, SmallObjectPerformanceCase)
	{
		ObjectPool<double> pool(10, false);
		ASSERT_TRUE(!pool.empty());

		HighResTimer timer;

		// naive case
		{
			const int count = 10000;
			double timeSum = 0;

			const long start = timer.getTimeInNanos();
			for (int i = 0; i < 10000; ++i)
			{
				double* value = new double();
				delete value;
			}
			const long end = timer.getTimeInNanos();
			timeSum += end - start;

			std::cout << "SmallObject Native Time per retrieval: " << (timeSum / count) << std::endl;
		}

		// native pointer case
		{
			const int count = 10000;
			double timeSum = 0;

			const long start = timer.getTimeInNanos();
			for (int i = 0; i < 10000; ++i)
			{
				double* value = pool.get();
				pool.release(value);
			}
			const long end = timer.getTimeInNanos();
			timeSum += end - start;

			std::cout << "SmallObject ptr Time per retrieval: " << (timeSum / count) << std::endl;
		}

		// shared ptr case
		{
			const int count = 10000;
			double timeSum = 0;
			std::shared_ptr<double> value;

			const long start = timer.getTimeInNanos();
			for (int i = 0; i < 10000; ++i)
			{
				value = pool.getShared();
			}
			const long end = timer.getTimeInNanos();
			timeSum += end - start;

			std::cout << "SmallObject std::shared_ptr Time per retrieval: " << (timeSum / count) << std::endl;
		}

	}

	TEST(ObjectPoolTest, BigObjectPerformanceCase)
	{
		struct BigObject
		{
			std::array<double, 30> fields;
		};

		ObjectPool<BigObject> pool(100, false);
		ASSERT_TRUE(!pool.empty());

		HighResTimer timer;

		// naive case
		{
			const int count = 10000;
			double timeSum = 0;

			const long start = timer.getTimeInNanos();
			for (int i = 0; i < 10000; ++i)
			{
				BigObject* value = new BigObject();
				delete value;
			}
			const long end = timer.getTimeInNanos();
			timeSum += end - start;

			std::cout << "BigObject Native Time per retrieval: " << (timeSum / count) << std::endl;
		}

		// native pointer case
		{
			const int count = 10000;
			double timeSum = 0;

			const long start = timer.getTimeInNanos();
			for (int i = 0; i < 10000; ++i)
			{
				BigObject* value = pool.get();
				pool.release(value);
			}
			const long end = timer.getTimeInNanos();
			timeSum += end - start;

			std::cout << "BigObject ptr Time per retrieval: " << (timeSum / count) << std::endl;
		}

		// shared ptr case
		{
			const int count = 10000;
			double timeSum = 0;
			std::shared_ptr<BigObject> value;

			const long start = timer.getTimeInNanos();
			for (int i = 0; i < 10000; ++i)
			{
				value = pool.getShared();
			}
			const long end = timer.getTimeInNanos();
			timeSum += end - start;

			std::cout << "BigObject std::shared_ptr Time per retrieval: " << (timeSum / count) << std::endl;
		}

	}

}
