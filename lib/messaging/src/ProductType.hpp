/*
 * ProductType.hpp
 *
 *  Created on: Sep 27, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_PRODUCTTYPE_HPP_
#define LIB_MESSAGING_SRC_PRODUCTTYPE_HPP_

#include <cstdint>

namespace mm
{
	//
	// Defines the product type.
	//
	enum class ProductType : std::int64_t
	{
		// Stock.
		STOCK = 0,

		// Equity index.
		INDEX = 1,

		// FX rate.
		FX = 2,

		// Commodity (cash).
		COMMODITY = 3,

		// ETF.
		ETF = 4,

		// Future (of any underlying).
		FUTURE = 5,

		// European option.
		EUROPEAN = 6,

		// American option.
		AMERICAN = 7,

		// Government bond
		GVMT_BOND = 8,
	};

	//
	// The utility functions for the productType enum.
	//
	struct ProductTypeUtil
	{
		//
		// Get the product type from its string representation.
		//
		// value : The string name of the product type.
		//
		// return : The product type.
		//
		static ProductType fromString(const std::string& value)
		{
			static const std::pair<std::string, ProductType> VALUES[] = {
					{"STOCK", ProductType::STOCK},
					{"INDEX", ProductType::INDEX},
					{"FX", ProductType::FX},
					{"COMMODITY", ProductType::COMMODITY},
					{"ETF", ProductType::ETF},
					{"FUTURE", ProductType::FUTURE},
					{"EUROPEAN", ProductType::EUROPEAN},
					{"AMERICAN", ProductType::AMERICAN},
					{"GVMT_BOND", ProductType::GVMT_BOND},
			};

			for (std::size_t i = 0; i < 13; ++i)
			{
				if (value == VALUES[i].first)
				{
					return VALUES[i].second;
				}
			}

			throw std::runtime_error("Cannot get product type from string value: " + value);
		}
	};
}



#endif /* LIB_MESSAGING_SRC_PRODUCTTYPE_HPP_ */
