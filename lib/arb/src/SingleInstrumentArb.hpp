/*
 * SingleFutureArb.hpp
 *
 *  Created on: Sep 19, 2018
 *      Author: suoalex
 */

#ifndef LIB_ARB_SRC_SINGLEINSTRUMENTARB_HPP_
#define LIB_ARB_SRC_SINGLEINSTRUMENTARB_HPP_

#include <memory>

#include <boost/circular_buffer.hpp>

#include <DispatchableService.hpp>
#include <MarketDataMessage.hpp>
#include <IConsumer.hpp>
#include <IPublisher.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>

namespace mm
{
	//
	// The strategy for single instrument arbitrage.
	//
	class SingleInstrumentArb :
			public DispatchableService,
			public IConsumer<MarketDataMessage>,
			public IConsumer<OrderSummaryMessage>,
			public IPublisher<OrderMessage>
	{
	public:

		//
		// Constructor.
		//
		// dispatchKey : The dispatch key.
		// dispatcher : The dispatcher.
		// serviceContext : context for pub/sub and services.
		// instrumentId : The instrument ID.
		// sampleCount : Market data sample count.
		//
		SingleInstrumentArb(
				const KeyType dispatchKey,
				const std::shared_ptr<Dispatcher> dispatcher,
				const std::shared_ptr<ServiceContext> serviceContext,
				const std::int64_t instrumentId,
				const std::size_t sampleCount);

		virtual ~SingleInstrumentArb();

		//
		// Service interface implementation
		//
		virtual bool start() override;
		virtual void stop() override;

		//
		// consumer interface implementation
		//
		virtual void consume(const std::shared_ptr<const MarketDataMessage>& message) override;
		virtual void consume(const std::shared_ptr<const OrderSummaryMessage>& message) override;

	private:

		//
		// Processing the core logic.
		//
		void process();

		// The instrument ID this algo is working on.
		const std::int64_t instrumentId;

		// The chrological messages.
		boost::circular_buffer<std::shared_ptr<const MarketDataMessage> > messages;

		// The ID of the live order in market - assuming only 1 wave allowed.
		std::int64_t liveOrderId;
	};
}



#endif /* LIB_ARB_SRC_SINGLEINSTRUMENTARB_HPP_ */
