/*
 * ProductMessage.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include "ProductMessage.hpp"

namespace mm
{
	ProductMessage::ProductMessage() :
			id(0),
			underlyerId(0),
			exchange(Exchange::SHFE),
			productType(ProductType::STOCK),
			currency(Currency::CNY),
			callPut(CallPutType::CALL),
			lotSize(1),
			listingDate(0),
			lastTradingDate(0),
			expiryDate(0),
			strike(0.0),
			contractRatio(1.0),
			conversionRatio(1.0),
			constituentCount(0)
	{
	}

	bool ProductMessage::equals (const ProductMessage& rhs) const
	{
		return id == rhs.id &&
				underlyerId == rhs.underlyerId &&
				exchange == rhs.exchange &&
				productType == rhs.productType &&
				currency == rhs.currency &&
				lotSize == rhs.lotSize &&
				listingDate == rhs.listingDate &&
				lastTradingDate == rhs.lastTradingDate &&
				expiryDate == rhs.expiryDate &&
				strike == rhs.strike &&
				contractRatio == rhs.contractRatio &&
				conversionRatio == rhs.conversionRatio &&
				constituentCount == rhs.constituentCount;
	}

}


