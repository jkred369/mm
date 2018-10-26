/*
 * SubscriptionTest.cpp
 *
 *  Created on: Oct 26, 2018
 *      Author: suoalex
 */

#include <gtest/gtest.h>

#include "Subscription.hpp"

namespace mm
{
	TEST(SubscriptionTest, EqualCase)
	{
		const Subscription sub1 = {SourceType::ARB, DataType::ALGO_COMMAND, 1};
		const Subscription sub2 = {SourceType::ALL, DataType::ALGO_COMMAND, 1};

		ASSERT_TRUE(sub1 == sub1);
		ASSERT_TRUE(sub1 != sub2);

		const Subscription sub3 = {SourceType::ARB, DataType::ALL, 1};
		ASSERT_TRUE(sub1 != sub3);

		const Subscription sub4 = {SourceType::ARB, DataType::ALGO_COMMAND, ALL_ID};
		ASSERT_TRUE(sub1 != sub4);

		const Subscription sub5 = sub1;
		ASSERT_TRUE(sub1 == sub5);
	}

	TEST(SubscriptionTest, HashCase)
	{
		const Subscription sub1 = {SourceType::ARB, DataType::ALGO_COMMAND, 1};
		const Subscription sub2 = {SourceType::ALL, DataType::ALGO_COMMAND, 1};

		std::hash<Subscription> hash;

		ASSERT_TRUE(hash(sub1) == hash(sub1));
		ASSERT_TRUE(hash(sub1) != hash(sub2));

		const Subscription sub3 = {SourceType::ARB, DataType::ALL, 1};
		ASSERT_TRUE(hash(sub1) != hash(sub3));

		const Subscription sub4 = {SourceType::ARB, DataType::ALGO_COMMAND, ALL_ID};
		ASSERT_TRUE(hash(sub1) != hash(sub4));

		const Subscription sub5 = sub1;
		ASSERT_TRUE(hash(sub1) == hash(sub5));
	}

}
