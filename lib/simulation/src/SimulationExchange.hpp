/*
 * SimulationExchange.hpp
 *
 *  Created on: 7 Nov 2018
 *      Author: alexsuo
 */

#ifndef LIB_SIMULATION_SRC_SIMULATIONEXCHANGE_HPP_
#define LIB_SIMULATION_SRC_SIMULATIONEXCHANGE_HPP_

#include <unordered_map>
#include <vector>

#include <ExecutionReportMessage.hpp>
#include <MarketDataMessage.hpp>
#include <IConsumer.hpp>
#include <ObjectPool.hpp>
#include <OrderManager.hpp>
#include <Product.hpp>
#include <PublisherAdapter.hpp>

namespace mm
{
	//
	// This class defines a simulation exchange.
	//
	// This class is the combination of a market data session and an order session due to
	// the market data must be shared in both side for decision.
	//
	class SimulationExchange :
			public OrderManager<SimulationExchange, NullObjectPool>,
			public PublisherAdapter<MarketDataMessage>,
			public PublisherAdapter<ExecutionReportMessage>,
			public IConsumer<Product>
	{
	public:

		SimulationExchange(
				const KeyType dispatchKey,
				const std::string serviceName,
				ServiceContext& serviceContext,
				const std::string productServiceName);

		// virtual destructor.
		virtual ~SimulationExchange();

		//
		// service interface.
		//
		virtual bool start() override;
		virtual void stop() override;

		//
		// publisher functionality for market data.
		//
		virtual bool subscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer) override;
		virtual void unsubscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer) override;

		virtual bool subscribe(const Subscription& subscription, IConsumer<ExecutionReportMessage>* consumer) override;
		virtual void unsubscribe(const Subscription& subscription, IConsumer<ExecutionReportMessage>* consumer) override;

		//
		// consumer interface
		//
		virtual void consume(const std::shared_ptr<const Product>& message) override;

		//
		// Send new order to exchange.
		//
		// message : The message representing new order.
		//
		void sendOrder(const std::shared_ptr<const OrderMessage>& message);

		//
		// Cancel order.
		//
		// message : The message representing order to cancel.
		//
		void cancel(const std::shared_ptr<const OrderMessage>& message);

	private:

		// The int value for bid
		static constexpr int BID = toValue(Side::BID);

		// The int value for ask
		static constexpr int ASK = toValue(Side::ASK);

		//
		// Check if execution can be done on the given market data update.
		//
		// marketData : The last market data update for the instrument.
		//
		void determineExecution(const std::shared_ptr<MarketDataMessage>& marketData);

		// The logger for this class.
		static Logger logger;

		// The dispatcher.
		Dispatcher& dispatcher;

		// The scheduler used.
		Scheduler& scheduler;

		// The pool used for market data messages.
		NullObjectPool<MarketDataMessage> marketDataPool;

		// The pool for execution report messages.
		NullObjectPool<ExecutionReportMessage> executionReportPool;

		// The map where key is the instrument ID and value is the lot size for the instrument.
		std::unordered_map<std::int64_t, std::int64_t> lotSizeMap;

		// The map where key is the instrument ID and value is a set of the live orders.
		std::unordered_map<std::int64_t, std::unordered_map<std::int64_t, OrderSummaryMessage> > instrumentOrderMap;

		// The map with the last market data for each instrument.
		std::unordered_map<std::int64_t, std::shared_ptr<MarketDataMessage> > lastMarketDataMap;
	};
}



#endif /* LIB_SIMULATION_SRC_SIMULATIONEXCHANGE_HPP_ */
