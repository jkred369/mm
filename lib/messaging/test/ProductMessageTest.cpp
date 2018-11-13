/*
 * ProductMessageTest.cpp
 *
 *  Created on: 13 Nov 2018
 *      Author: alexsuo
 */

#include <fstream>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <gtest/gtest.h>

#include <StringBuffer.hpp>
#include <StringUtil.hpp>

#include "ProductMessage.hpp"

namespace mm
{
	TEST(ProductMessageTest, DISABLED_OutputCase)
	{
		bool headerRead = false;
		std::int64_t idCounter = 0;

		std::ifstream is("/home/alexsuo/workspaces/mm/data/code.csv");
		std::ofstream os("/home/alexsuo/workspaces/mm/data/product.txt");

		for (std::string line; std::getline(is, line); )
		{
			// obtain valid line
			StringUtil::trim(line);
			if (line.empty() || line[0] == '#')
			{
				continue;
			}

			if (!headerRead)
			{
				headerRead = true;
				continue;
			}

			// split into items
			std::vector<std::string> items;
			boost::split(items, line, boost::is_any_of("|"));
			for (std::string& item : items)
			{
				StringUtil::trim(item);
			}

			// fill the product
			ProductMessage product;

			product.id = ++idCounter;
			product.exchange = ExchangeUtil::fromString(items[0]);
			product.currency = Currency::CNY;
			product.conversionRatio = 1.0;
			product.contractRatio = 1.0;
			product.lotSize = 1;
			product.productType = ProductType::COMMODITY;
			product.symbol = items[3];

			// output
			StringBuffer buffer;
			ASSERT_TRUE(product.serialize(buffer));
			os << buffer;
		}

		os.flush();
		os.close();
		is.close();
	}
}


