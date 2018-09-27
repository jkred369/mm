/*
 * Product.hpp
 *
 *  Created on: Sep 27, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_PRODUCTMESSAGE_HPP_
#define LIB_MESSAGING_SRC_PRODUCTMESSAGE_HPP_

#include "CallPutType.hpp"
#include "Currency.hpp"
#include "Exchange.hpp"
#include "FieldDefinition.hpp"
#include "Message.hpp"
#include "ProductType.hpp"

namespace mm
{
	//
	// This class defines a message strong enough for holding cash, delta 1 and simple vanillas.
	//
	class ProductMessage : public Message
	{
	public:

		// The ID for underlyer ID as in no underlyer.
		static constexpr std::int64_t INVALID_ID = -1;

		// The instrument ID.
		std::int64_t id;

		// The underlyer ID.
		std::int64_t underlyerId;

		// Symbol as the symbol in exchange where its traded
		SymbolType symbol;

		// exchange ID
		Exchange exchange;

		// product type
		ProductType productType;

		// currency in which the product is quoted / traded
		Currency currency;

		// call-put type
		CallPutType callPut;

		// lot size
		std::int64_t lotSize;

		// first trading date.
		Date listingDate;

		// last trading date
		Date lastTradingDate;

		// expiry date - epoch if no expiry
		Date expiryDate;

		// strike. 0 if no strike.
		double strike;

		// contract ratio as in contract to currency ratio.
		double contractRatio;

		// conversion ratio as in product to underlying ratio (if any)
		double conversionRatio;
	};
}



#endif /* LIB_MESSAGING_SRC_PRODUCTMESSAGE_HPP_ */
