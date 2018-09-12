#include <cstdint>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include "SingletonInstance.hpp"

namespace mm
{
	struct TestValue
	{
	protected:

		TestValue(char value) : value(value)
		{
		}

	public:

		char value;

		bool operator == (const TestValue& rhs) const
		{
			return value == rhs.value;
		}

		std::size_t hashValue() const
		{
			return std::hash<char>{}(value);
		}

		friend class SingletonInstance<TestValue>;
	};

	typedef SingletonInstance<TestValue> TestSingletonInstance;


	TEST(SingletonInstanceTest, BasicCase)
	{
		const TestValue& value1 = TestSingletonInstance::getInstance('1');
		const TestValue& value2 = TestSingletonInstance::getInstance('1');

		ASSERT_TRUE(&value1 == &value2);
		ASSERT_TRUE(value1 == value2);

		const TestValue& value3 = TestSingletonInstance::getInstance('2');
		const TestValue& value4 = TestSingletonInstance::getInstance('2');

		ASSERT_TRUE(&value3 == &value4);
		ASSERT_TRUE(value3 == value4);

		const TestValue& value5 = TestSingletonInstance::getInstance('1');
		ASSERT_TRUE(&value1 == &value5);
	}

}

