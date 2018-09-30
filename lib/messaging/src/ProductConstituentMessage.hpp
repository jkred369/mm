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

		//
		// The template method for serialize to buffer.
		// used to provide a uniform behavior across buffer types.
		//
		// buffer : The buffer to write to.
		//
		// return : true if the serialization done successfully.
		//
		template<typename WriteBuffer> bool serialize(WriteBuffer& buffer) const
		{
			buffer << id << weight;
			return buffer.getError();
		}

		//
		// The template method for deserialize from buffer.
		// used to provide a uniform behavrior across buffer types.
		//
		// buffer : The buffer to read from.
		//
		// return : true if the deserialization done successfully.
		//
		template<typename ReadBuffer> bool deserialize(ReadBuffer& buffer)
		{
			buffer >> id >> weight;
			return buffer.getError();
		}

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

		//
		// The template method for serialize to buffer.
		// used to provide a uniform behavior across buffer types.
		//
		// buffer : The buffer to write to.
		//
		// return : true if the serialization done successfully.
		//
		template<typename WriteBuffer> bool serialize(WriteBuffer& buffer) const
		{
			if (UNLIKELY(!Message::serialize(buffer)))
			{
				return false;
			}

			buffer << id << count;
			for (int i = 0; i < count; ++i)
			{
				buffer << constituents[i];
			}

			return buffer.getError();
		}

		//
		// The template method for deserialize from buffer.
		// used to provide a uniform behavrior across buffer types.
		//
		// buffer : The buffer to read from.
		//
		// return : true if the deserialization done successfully.
		//
		template<typename ReadBuffer> bool deserialize(ReadBuffer& buffer)
		{
			if (UNLIKELY(!Message::deserialize(buffer)))
			{
				return false;
			}

			buffer >> id >> count;
			for (int i = 0; i < count; ++i)
			{
				buffer >> constituents[i];
			}

			return buffer.getError();
		}

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
