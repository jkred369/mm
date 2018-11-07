/*
 * SimulationExchange.cpp
 *
 *  Created on: 7 Nov 2018
 *      Author: alexsuo
 */

#include "SimulationExchange.hpp"

mm::Logger mm::SimulationExchange::logger;

namespace mm
{
	SimulationExchange::SimulationExchange(
			const KeyType dispatchKey,
			const std::string serviceName,
			ServiceContext& serviceContext,
			const std::string productServiceName) :
		OrderManager<SimulationExchange, NullObjectPool>(dispatchKey, serviceName, serviceContext, *this),
		PublisherAdapter<MarketDataMessage> (serviceContext.getDispatcher()),
		PublisherAdapter<ExecutionReportMessage> (serviceContext.getDispatcher()),
		dispatcher(serviceContext.getDispatcher()),
		scheduler(serviceContext.getScheduler()),
		marketDataPool(POOL_SIZE),
		executionReportPool(POOL_SIZE)
	{
		static_assert(BID >= 0, "Bid index must be positive.");
		static_assert(ASK >= 0, "Ask index must be positive.");

		// TODO : Make the product update dynamic.
		{
			ProductService* productService;
			if (!serviceContext.getService(productServiceName, productService))
			{
				LOGERR("Order session failed to get product service from service context");
				throw std::runtime_error("Failed to create Femas order session. Cannot find product service with name: " + productServiceName);
			}

			productService->initSnapshot(this);
			LOGINFO("Symbol map created with {} products.", idMap.size());
		}
	}

	SimulationExchange::~SimulationExchange()
	{
	}

	bool SimulationExchange::start()
	{
		if (!OrderManager<SimulationExchange, NullObjectPool>::start())
		{
			return false;
		}

		// TODO: start the market data broadcasting

		return true;
	}

	void SimulationExchange::stop()
	{
		OrderManager<SimulationExchange, NullObjectPool>::stop();
	}

	bool SimulationExchange::subscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer)
	{
	}

	void SimulationExchange::unsubscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer)
	{
	}

	void SimulationExchange::consume(const std::shared_ptr<const Product>& message)
	{
		lotSizeMap[message->content.id] = message->content.lotSize;
		instrumentOrderMap[message->content.id] = std::vector<OrderSummaryMessage> ();
	}

	void SimulationExchange::sendOrder(const std::shared_ptr<const OrderMessage>& message)
	{
	}

	void SimulationExchange::cancel(const std::shared_ptr<const OrderMessage>& message)
	{
	}

}
