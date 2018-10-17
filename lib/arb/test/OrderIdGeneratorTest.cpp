/*
 * OrderIdGeneratorTest.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: suoalex
 */

#include <cstring>

#include <boost/lexical_cast.hpp>

#include <gtest/gtest.h>

#include "AlgoUtil.hpp"
#include "OrderIdGenerator.hpp"

namespace mm
{
	TEST(OrderIdGeneratorTest, SimpleCase)
	{
		OrderIdGenerator& generator = AlgoUtil::getDefaultOrderIdGenerator();
		std::int64_t id = generator.generate(0);

		ASSERT_TRUE(id > 0);
	}

	TEST(OrderIdGeneratorTest, ErrorCase)
	{
		OrderIdGenerator& generator = AlgoUtil::getDefaultOrderIdGenerator();

		std::int64_t id1 = generator.generate(-1);
		ASSERT_TRUE(id1 == OrderIdGenerator::invalidOrderId);

		std::int64_t id2 = generator.generate(100);
		ASSERT_TRUE(id2 == OrderIdGenerator::invalidOrderId);
	}

	TEST(OrderIdGeneratorTest, IncrementalCase)
	{
		OrderIdGenerator& generator = AlgoUtil::getDefaultOrderIdGenerator();
		std::int64_t id1 = generator.generate(20);
		std::int64_t id2 = generator.generate(5);

		ASSERT_TRUE(id1 > 0);
		ASSERT_TRUE(id2 > 0);
		ASSERT_TRUE(id2 > id1);

		std::string id1String = boost::lexical_cast<std::string> (id1);
		std::string id2String = boost::lexical_cast<std::string> (id2);

		ASSERT_TRUE(std::strcmp(id1String.c_str(), id2String.c_str()) < 0);
	}
}
