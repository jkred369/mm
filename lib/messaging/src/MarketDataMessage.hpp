/*
 * MarketDataMessage.hpp
 *
 *  Created on: Sep 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_MARKETDATAMESSAGE_HPP_
#define LIB_MESSAGING_SRC_MARKETDATAMESSAGE_HPP_

#include <cstdint>

#include "FieldDefinition.hpp"
#include "Message.hpp"
#include "Side.hpp"

namespace mm
{
	//
	// The struct for market data level.
	//
	struct MarketDataLevel
	{
		//
		// default constructor.
		//
		MarketDataLevel();

		//
		// Check for equalness of level.
		//
		// rhs : The other message.
		//
		// return : True if the other message is identical.
		//
		bool equals(const MarketDataLevel& rhs) const;

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
			buffer << price << qty;
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
			buffer >> price >> qty;
			return buffer.getError();
		}

		// the price.
		double price;

		// the avaiable qty
		std::int64_t qty;
	};

	DEFINE_OPERATORS(MarketDataLevel)

	//
	// The market data message.
	//
	class MarketDataMessage : public Message
	{
	public:

		// The maximum market data depth.
		static constexpr size_t MAX_DEPTH = 5;

		//
		// Default constructor.
		//
		MarketDataMessage();

		//
		// Check for equalness of market data.
		//
		// rhs : The other message.
		//
		// return : True if the other message is identical.
		//
		bool equals(const MarketDataMessage& rhs) const;

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

			buffer << instrumentId << open << close << high << low << last << highLimit << lowLimit << volume << turnover;

			for (std::size_t i = 0; i < 2; ++i)
			{
				for (std::size_t j = 0; j < MAX_DEPTH; ++j)
				{
					buffer << levels[i][j];
				}
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

			buffer >> instrumentId >> open >> close >> high >> low >> last >> highLimit >> lowLimit >> volume >> turnover;

			for (std::size_t i = 0; i < 2; ++i)
			{
				for (std::size_t j = 0; j < MAX_DEPTH; ++j)
				{
					buffer >> levels[i][j];
				}
			}

			return buffer.getError();
		}

		// Instrument ID.
		std::int64_t instrumentId;

		// open price
		double open;

		// last close price.
		double close;

		// day high price.
		double high;

		// day low price.
		double low;

		// last price.
		double last;

		// upper limit price.
		double highLimit;

		// lower limit price.
		double lowLimit;

		// cumulative volume.
		std::int64_t volume;

		// cumulative turnover.
		double turnover;

		// Market depth
		MarketDataLevel levels[2][MAX_DEPTH];
	};

	DEFINE_OPERATORS(MarketDataMessage)
}



#endif /* LIB_MESSAGING_SRC_MARKETDATAMESSAGE_HPP_ */
