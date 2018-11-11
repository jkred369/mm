/*
 * SimulationExchange.cpp
 *
 *  Created on: 7 Nov 2018
 *      Author: alexsuo
 */

#include <algorithm>

#include <ProductService.hpp>

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
			LOGINFO("Symbol map created with {} products.", lotSizeMap.size());
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
			return PublisherAdapter<MarketDataMessage>::subscribe(subscription, consumer);
		}

		return false;
	}

	void SimulationExchange::unsubscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer)
	{
		if (UNLIKELY(lotSizeMap.find(subscription.key) != lotSizeMap.end()))
		{
			return;
		}

		if (subscription.dataType == DataType::MARKET_DATA)
		{
			PublisherAdapter<MarketDataMessage>::unsubscribe(subscription, consumer);
		}
	}

	bool SimulationExchange::subscribe(const Subscription& subscription, IConsumer<ExecutionReportMessage>* consumer)
	{
		if (UNLIKELY(lotSizeMap.find(subscription.key) != lotSizeMap.end()))
		{
			return false;
		}

		if (subscription.dataType == DataType::EXEC_REPORT)
		{
			return PublisherAdapter<ExecutionReportMessage>::subscribe(subscription, consumer);
		}

		return false;
	}

	void SimulationExchange::unsubscribe(const Subscription& subscription, IConsumer<ExecutionReportMessage>* consumer)
	{
		if (UNLIKELY(lotSizeMap.find(subscription.key) != lotSizeMap.end()))
		{
			return;
		}

		if (subscription.dataType == DataType::EXEC_REPORT)
		{
			PublisherAdapter<ExecutionReportMessage>::unsubscribe(subscription, consumer);
		}
	}

	void SimulationExchange::consume(const std::shared_ptr<const Product>& message)
	{
		const ProductMessage& product = message->getContent();

		lotSizeMap[product.id] = product.lotSize;
		instrumentOrderMap[product.id] = std::unordered_map<std::int64_t, OrderSummaryMessage> ();
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

			OrderSummaryMessage summary;
			summary.instrumentId = message->instrumentId;
			summary.orderId = message->orderId;
			summary.price = message->price;
			summary.side = message->side;
			summary.status = response->status;
			summary.strategyId = message->strategyId;
			summary.totalQty = message->totalQty;
			summary.tradedNotional = 0;
			summary.tradedQty = 0;

			instrumentOrderMap[message->instrumentId].insert(std::make_pair(summary.orderId, summary));
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
		std::shared_ptr<ExecutionReportMessage> response = executionReportPool.getShared();
		response->instrumentId = message->instrumentId;
		response->orderId = message->orderId;
		response->side = message->side;

		// sanity check
		if (UNLIKELY(instrumentOrderMap.find(message->instrumentId) == instrumentOrderMap.end()))
		{
			response->status = OrderStatus::CANCEL_REJECTED;
		}
		else
		{
			std::unordered_map<std::int64_t, OrderSummaryMessage>& orders = instrumentOrderMap[message->instrumentId];
			auto it = std::find_if(orders.begin(), orders.end(), [&] (const std::pair<std::int64_t, OrderSummaryMessage>& pair) {
				return pair.first == message->orderId;
			});

			if (it == orders.end() || OrderStatusUtil::completed(it->second.status))
			{
				response->status = OrderStatus::CANCEL_REJECTED;
			}
			else
			{
				response->status = OrderStatus::CANCELLED;
				orders.erase(it);
			}
		}

		const Subscription subscription(SourceType::ALL, DataType::EXEC_REPORT, message->strategyId);
		PublisherAdapter<ExecutionReportMessage>::publish(subscription, response);
	}

	void SimulationExchange::determineExecution(const std::shared_ptr<MarketDataMessage>& marketData)
	{
		std::unordered_map<std::int64_t, OrderSummaryMessage>& orderMap = instrumentOrderMap[marketData->instrumentId];
		for (auto it = orderMap.begin(); it != orderMap.end(); )
		{
			OrderSummaryMessage& summary = it->second;
			if ((summary.side == Side::BID && summary.price >= marketData->levels[toValue(Side::ASK)][0].price) ||
				(summary.side == Side::ASK && summary.price <= marketData->levels[toValue(Side::BID)][0].price))
			{

			}

		}
	}

}
