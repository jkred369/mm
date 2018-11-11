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
		if (UNLIKELY(lotSizeMap.find(subscription.key) != lotSizeMap.end()))
		{
			return false;
		}

		if (subscription.dataType == DataType::MARKET_DATA)
		{
			return PublisherAdapter<MarketData>::subscribe(subscription, consumer);
		}
		else if (subscription.dataType == DataType::EXEC_REPORT)
		{
			return PublisherAdapter<ExeutionReportMessage>::subscribe(subscription, consumer);
		}

		return OrderManager<SimulationExchange, NullObjectPool>::subscribe(subscription, consumer);
	}

	void SimulationExchange::unsubscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer)
	{
		if (UNLIKELY(lotSizeMap.find(subscription.key) != lotSizeMap.end()))
		{
			return;
		}

		if (subscription.dataType == DataType::MARKET_DATA)
		{
			PublisherAdapter<MarketData>::unsubscribe(subscription, consumer);
		}
		else if (subscription.dataType == DataType::EXEC_REPORT)
		{
			PublisherAdapter<ExeutionReportMessage>::unsubscribe(subscription, consumer);
		}
		else
		{
			OrderManager<SimulationExchange, NullObjectPool>::unsubscribe(subscription, consumer);
		}
	}

	void SimulationExchange::consume(const std::shared_ptr<const Product>& message)
	{
		lotSizeMap[message->content.id] = message->content.lotSize;
		instrumentOrderMap[message->content.id] = std::vector<OrderSummaryMessage> ();
	}

	void SimulationExchange::sendOrder(const std::shared_ptr<const OrderMessage>& message)
	{
		std::shared_ptr<ExecutionReportMessage> response = executionReportPool.getShared();
		response->instrumentId = message->instrumentId;
		response->orderId = message->orderId;
		response->side = message->side;

		// sanity check
		if (UNLIKELY(instrumentOrderMap.find(message->instrumentId) == instrumentOrderMap.end()))
		{
			response->status = OrderStatus::NEW_REJECTED;
		}
		else
		{
			response->status = OrderStatus::LIVE;

			// this is the state keeper for the exchange so we don't resolve to the pool
			std::shared_ptr<OrderSummaryMessage> summary = std::make_shared<OrderSummaryMessage>();
			summary->instrumentId = message->instrumentId;
			summary->orderId = message->orderId;
			summary->price = message->price;
			summary->side = message->side;
			summary->status = response->status;
			summary->strategyId = message->strategyId;
			summary->totalQty = message->totalQty;
			summary->tradedNotional = 0;
			summary->tradedQty = 0;

			instrumentOrderMap[message->instrumentId].push_back(summary);
		}

		const Subscription subscription(SourceType::ALL, DataType::EXEC_REPORT, message->strategyId);
		PublisherAdapter<ExecutionReportMessage>::publish(subscription, response);

		// check execution if any
		if (std::shared_ptr<MarketDataMessage> marketData = lastMarketDataMap[message->instrumentId])
		{
			determineExecution(marketData);
		}
	}

	void SimulationExchange::cancel(const std::shared_ptr<const OrderMessage>& message)
	{
	}

	void SimulationExchange::determineExecution(const std::shared_ptr<MarketDataMessage>& marketData)
	{
		for (std::shared_ptr<OrderSummaryMessage> summary : instrumentOrderMap[marketData->instrumentId])
		{

		}
	}

}
