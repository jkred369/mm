/*
 * MarketDataMessage.hpp
 *
 *  Created on: Sep 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_MARKETDATAMESSAGE_HPP_
#define LIB_MESSAGING_SRC_MARKETDATAMESSAGE_HPP_

#include <cstdint>

#include <FieldDefinition.hpp>
#include <Message.hpp>
#include <Side.hpp>

namespace mm
{
	//
	// The struct for market data level.
	//
	struct MarketDataLevel
	{
		double price;
		std::int64_t qty;
	};

	//
	// The market data message.
	//
	class MarketDataMessage : public Message
	{
	public:

		// The maximum market data depth.
		constexpr size_t MAX_DEPTH = 5;

		// Instrument ID.
		InstrumentIDType instrumentId;

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
}



#endif /* LIB_MESSAGING_SRC_MARKETDATAMESSAGE_HPP_ */
