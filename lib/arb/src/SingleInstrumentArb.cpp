/*
 * SingleInstrumentArb.cpp
 *
 *  Created on: Sep 19, 2018
 *      Author: suoalex
 */

#include <EnumType.hpp>
#include <Subscription.hpp>

#include "SingleInstrumentArb.hpp"

namespace mm
{
	SingleInstrumentArb::SingleInstrumentArb(
			const KeyType dispatchKey,
			const std::string serviceName,
			ServiceContext& serviceContext,
			const std::int64_t instrumentId,
			const std::size_t sampleCount) :
		DispatchableService(dispatchKey, serviceName, serviceContext),
		instrumentId(instrumentId),
		messages(sampleCount),
		liveOrderId(0)
	{
	}

	bool SingleInstrumentArb::start()
	{
		// call parent to do sanity check
		if (!DispatchableService::start())
		{
			return false;
		}

		// wire up all the subscriptions
		if (!serviceContext.subscribe(
				Subscription(SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, instrumentId),
				dynamic_cast<IConsumer<MarketDataMessage>*> (this)))
		{
			return false;
		}

		if (!serviceContext.subscribe(
				Subscription(SourceType::FEMAS_ORDER, DataType::ORDER_SUMMARY, instrumentId),
				dynamic_cast<IConsumer<OrderSummaryMessage>*> (this)))
		{
			return false;
		}

		return true;
	}

	void SingleInstrumentArb::stop()
	{
		// stop all the subscriptions
		serviceContext.unsubscribe(
				Subscription(SourceType::FEMAS_ORDER, DataType::ORDER_SUMMARY, instrumentId),
				dynamic_cast<IConsumer<OrderSummaryMessage>*> (this));

		serviceContext.unsubscribe(
				Subscription(SourceType::FEMAS_MARKET_DATA, DataType::MARKET_DATA, instrumentId),
				dynamic_cast<IConsumer<MarketDataMessage>*> (this));

		// delegate super class
		DispatchableService::stop();
	}

	void SingleInstrumentArb::consume(const std::shared_ptr<const MarketDataMessage>& message)
	{
		messages.push_back(message);
	}

	void SingleInstrumentArb::consume(const std::shared_ptr<const OrderSummaryMessage>& message)
	{
		if (message->orderId == liveOrderId && OrderStatusUtil::completed(message->status))
		{
			liveOrderId = 0;
		}
	}

	void SingleInstrumentArb::process()
	{
		// sanity checks
		if (!messages.full())
		{
			return;
		}

		//
		// magic to determine if we should generate a new order...
		//

		// assuming we have decided to generate a new order and its a buy
		// TODO: a pool need to be used here, will do later
		std::shared_ptr<OrderMessage> order = std::make_shared<OrderMessage>();

		// TODO: use an order ID generator
		order->orderId = 1;
		order->instrumentId = instrumentId;
		order->side = Side::ASK;
		order->totalQty = 1;
		order->price = messages.back()->levels[toValue(Side::ASK)][0].price;
		order->status = OrderStatus::LIVE;

		const Subscription subscription = {SourceType::ARB, DataType::NEW_ORDER, instrumentId};
		publish(subscription, order);
	}

}
