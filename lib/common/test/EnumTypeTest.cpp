/*
 * EnumTypeTest.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include <gtest/gtest.h>

#include "EnumType.hpp"

namespace mm
{
	enum class DummyEnum : std::int64_t
	{
		FIRST = 1,

		SECOND = 2,
	};

	TEST(EnumTypeTest, ConvertCase)
	{
		ASSERT_TRUE(DummyEnum::FIRST == fromValue<DummyEnum>(1));
		ASSERT_TRUE(2 == toValue(DummyEnum::SECOND));
	}

}


