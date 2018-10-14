/*
 * SingleInstrumentArb.cpp
 *
 *  Created on: Sep 19, 2018
 *      Author: suoalex
 */

#include <EnumType.hpp>
#include <Subscription.hpp>

#include "AlgoUtil.hpp"
#include "SingleInstrumentArb.hpp"

namespace mm
{
	SingleInstrumentArb::SingleInstrumentArb(
			const KeyType dispatchKey,
			const std::string serviceName,
			ServiceContext& serviceContext,
			const std::int64_t strategyId,
			const std::int64_t instrumentId,
			const std::size_t sampleCount) :
		DispatchableService(dispatchKey, serviceName, serviceContext),
		strategyId(strategyId),
		instrumentId(instrumentId),
		orderPool(orderPoolSize),
		orderIdGenerator(AlgoUtil::createOrderIdGenerator(strategyId)),
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
				Subscription(SourceType::FEMAS_ORDER, DataType::ORDER_SUMMARY, strategyId),
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
		std::shared_ptr<OrderMessage> order = orderPool.getShared();

		order->orderId = orderIdGenerator.generate();
		order->instrumentId = instrumentId;
		order->side = Side::ASK;
		order->totalQty = 1;
		order->price = messages.back()->levels[toValue(Side::ASK)][0].price;
		order->status = OrderStatus::LIVE;

		const Subscription subscription = {SourceType::ARB, DataType::NEW_ORDER, strategyId};
		publish(subscription, order);
	}

}
