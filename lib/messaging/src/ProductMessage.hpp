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
	// Note that the constituents information is in another message in order not to over complicate the simple products.
	//
	class ProductMessage : public Message
	{
	public:

		//
		// Default constructor.
		//
		ProductMessage();

		//
		// Check for equalness of product message.
		//
		// rhs : The other product.
		//
		// return : True if the other message is identical.
		//
		bool equals(const ProductMessage& rhs) const;

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

		// constituent count - 0 for any product apart from basket and index.
		std::int64_t constituentCount;
	};

	//
	// Implement custom comparison for the product message.
	//
	// lhs : The left product message.
	// rhs : The right product message.
	//
	// return : true if the 2 product messages are identical.
	//
	inline bool operator == (const ProductMessage& lhs, const ProductMessage& rhs)
	{
		return lhs.equals(rhs);
	}

}



#endif /* LIB_MESSAGING_SRC_PRODUCTMESSAGE_HPP_ */
