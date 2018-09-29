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
	// This class defines the constituent information for basket and indices.
	//
	class ProductConstituentMessage : public Message
	{
		// Max size of constituents for the product.
		static constexpr std::size_t MAX_CONSTITUENTS = 2000;

		//
		// Default constructor.
		//
		ProductConstituentMessage();

		// The instrument ID.
		std::int64_t instrumentId;

		// The count of constituents.
		std::int64_t count;

		// The constituent IDs.
		std::array<std::int64_t, MAX_CONSTITUENTS> constituents;

		// The weight of the constituents.
		std::array<double, MAX_CONSTITUENTS> weights;
	};
}



#endif /* LIB_MESSAGING_SRC_PRODUCTCONSTITUENTMESSAGE_HPP_ */
