#include <cstring>
#include <memory>

#include <gtest/gtest.h>

#include "FixedSizeString.hpp"

namespace mm
{
	TEST(FixedSizeStringTest, DummyCase)
	{
		FixedSizeString<0> dummyString;

		ASSERT_TRUE(dummyString.stringSize() == 0);
		ASSERT_TRUE(dummyString.capacity() == 0);
		ASSERT_TRUE(dummyString.toString() == "");

		{
			char* value = dummyString.toStr();
			ASSERT_TRUE(std::strlen(value) == 0);

			delete value;
		}

		{
			char* buffer = new char[2];
			std::memset(buffer, 0, 2);

			dummyString.copy(buffer);
			ASSERT_TRUE(std::strlen(buffer) == 0);

			delete buffer;
		}

		{
			char* buffer = new char[2];
			std::memset(buffer, 2, 2);

			dummyString.copy(buffer);
			ASSERT_TRUE(std::strlen(buffer) == 0);

			delete buffer;
		}
	}

	TEST(FixedSizeStringTest, SimpleCase)
	{
		FixedSizeString<5> string;

		ASSERT_TRUE(string.stringSize() == 0);
		ASSERT_TRUE(string.capacity() == 5);
		ASSERT_TRUE(string.toString() == "");

		{
			char* value = string.toStr();
			ASSERT_TRUE(std::strlen(value) == 0);

			delete value;
		}

		{
			char* buffer = new char[5];
			std::memset(buffer, 0, 5);

			string.copy(buffer);
			ASSERT_TRUE(std::strlen(buffer) == 0);

			delete buffer;
		}

		string = "abc";

		ASSERT_TRUE(string.stringSize() == 3);
		ASSERT_TRUE(string.capacity() == 5);
		ASSERT_TRUE(string.toString() == "abc");

		{
			char* value = string.toStr();
			ASSERT_TRUE(std::strlen(value) == 3);
			ASSERT_TRUE(std::strcmp(value, "abc") == 0);

			delete value;
		}

		{
			char* buffer = new char[5];
			std::memset(buffer, 0, 5);

			string.copy(buffer);
			ASSERT_TRUE(std::strlen(buffer) == 3);
			ASSERT_TRUE(std::strcmp(buffer, "abc") == 0);

			delete buffer;
		}

	}

	TEST(FixedSizeStringTest, OverflowCase)
	{
		FixedSizeString<3> string;

		ASSERT_TRUE(string.stringSize() == 0);
		ASSERT_TRUE(string.capacity() == 3);
		ASSERT_TRUE(string.toString() == "");

		{
			char* value = string.toStr();
			ASSERT_TRUE(std::strlen(value) == 0);

			delete value;
		}

		{
			char* buffer = new char[5];
			std::memset(buffer, 0, 5);

			string.copy(buffer);
			ASSERT_TRUE(std::strlen(buffer) == 0);

			delete buffer;
		}

		string = "abcde";

		ASSERT_TRUE(string.stringSize() == 3);
		ASSERT_TRUE(string.capacity() == 3);
		ASSERT_TRUE(string.toString() == "abc");

		{
			char* value = string.toStr();
			ASSERT_TRUE(std::strlen(value) == 3);
			ASSERT_TRUE(std::strcmp(value, "abc") == 0);

			delete value;
		}

		{
			char* buffer = new char[5];
			std::memset(buffer, 0, 5);

			string.copy(buffer);
			ASSERT_TRUE(std::strlen(buffer) == 3);
			ASSERT_TRUE(std::strcmp(buffer, "abc") == 0);

			delete buffer;
		}

	}

	TEST(FixedSizeStringTest, UnderflowCase)
	{
		FixedSizeString<5> string;

		ASSERT_TRUE(string.stringSize() == 0);
		ASSERT_TRUE(string.capacity() == 5);
		ASSERT_TRUE(string.toString() == "");

		{
			char* value = string.toStr();
			ASSERT_TRUE(std::strlen(value) == 0);

			delete value;
		}

		{
			char* buffer = new char[5];
			std::memset(buffer, 0, 5);

			string.copy(buffer);
			ASSERT_TRUE(std::strlen(buffer) == 0);

			delete buffer;
		}

		string = "abc";

		ASSERT_TRUE(string.stringSize() == 3);
		ASSERT_TRUE(string.capacity() == 5);
		ASSERT_TRUE(string.toString() == "abc");

		{
			char* value = string.toStr();
			ASSERT_TRUE(std::strlen(value) == 3);
			ASSERT_TRUE(std::strcmp(value, "abc") == 0);

			delete value;
		}

		{
			char* buffer = new char[5];
			std::memset(buffer, 0, 5);

			string.copy(buffer);
			ASSERT_TRUE(std::strlen(buffer) == 3);
			ASSERT_TRUE(std::strcmp(buffer, "abc") == 0);

			delete buffer;
		}

	}

}



