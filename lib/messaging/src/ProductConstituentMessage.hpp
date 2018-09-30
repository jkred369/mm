/*
 * ProductConstituentMessage.hpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_PRODUCTCONSTITUENTMESSAGE_HPP_
#define LIB_MESSAGING_SRC_PRODUCTCONSTITUENTMESSAGE_HPP_

#include <array>

#include "Message.hpp"

namespace mm
{
	//
	// A constituent.
	//
	struct Constituent
	{
		//
		// Default constructor.
		//
		Constituent();

		//
		// Check for equality of constituent.
		//
		// rhs : The other product.
		//
		// return : True if the other message is identical.
		//
		bool equals(const Constituent& rhs) const;

		// ID of the constituent.
		std::int64_t id;

		// weight.
		double weight;
	};

	DEFINE_OPERATORS(Constituent)

	//
	// This class defines the constituent information for basket and indices.
	//
	class ProductConstituentMessage : public Message
	{
	public:

		// Max size of constituents for the product.
		static constexpr std::size_t MAX_CONSTITUENTS = 256;

		//
		// Default constructor.
		//
		ProductConstituentMessage();

		//
		// Check for equalness of product message.
		//
		// rhs : The other product.
		//
		// return : True if the other message is identical.
		//
		bool equals(const ProductConstituentMessage& rhs) const;

		// The instrument ID.
		std::int64_t id;

		// The count of constituents.
		std::int64_t count;

		// The constituents.
		std::array<Constituent, MAX_CONSTITUENTS> constituents;
	};

	DEFINE_OPERATORS(ProductConstituentMessage)
}



#endif /* LIB_MESSAGING_SRC_PRODUCTCONSTITUENTMESSAGE_HPP_ */
