/*
 * SimulationExchange.cpp
 *
 *  Created on: 7 Nov 2018
 *      Author: alexsuo
 */

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <fmt/format.h>

#include <ProductService.hpp>
#include <StringUtil.hpp>

#include "SimulationExchange.hpp"

const std::int64_t mm::SimulationExchange::LEAD_SLEEP_SECONDS = 5;
mm::Logger mm::SimulationExchange::logger;

namespace mm
{
	SimulationExchange::SimulationExchange(
			const KeyType dispatchKey,
			const std::string serviceName,
			ServiceContext& serviceContext,
			const std::string productServiceName,
			std::istream& marketDataStream) :
		OrderManager<SimulationExchange, NullObjectPool>(dispatchKey, serviceName, serviceContext, *this),
		PublisherAdapter<MarketDataMessage> (serviceContext.getDispatcher()),
		PublisherAdapter<ExecutionReportMessage> (serviceContext.getDispatcher()),
		dispatcher(serviceContext.getDispatcher()),
		scheduler(serviceContext.getScheduler()),
		executionIdCounter(0),
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

		// loading up the market data ticks - could be big
		loadMarketData(marketDataStream);

		// prepare publishing
		marketDataIt = marketDataMessages.begin();
		publishingTask = [&] () {

			// firstly do publish
			if (LIKELY(marketDataIt != marketDataMessages.end()))
			{
				const std::chrono::microseconds timestamp = marketDataIt->first;
				const std::shared_ptr<MarketDataMessage> message = marketDataIt->second;
				const Subscription subscription(SourceType::ALL, DataType::MARKET_DATA, message->instrumentId);

				lastMarketDataMap[message->instrumentId] = message;
				PublisherAdapter<MarketDataMessage>::publish(subscription, message);

				if (LIKELY(++marketDataIt != marketDataMessages.end()))
				{
					// schedule for the next round
					const std::chrono::microseconds delay = marketDataIt->first - timestamp;
					scheduler.schedule(DispatchKey::MARKET_DATA, publishingTask, delay);
				}
				else
				{
					LOGINFO("Market data replay finished.");
				}
			}
			else
			{
				// if we reach here it means nothing to replay at all
				LOGINFO("No market data to replay. Market data replay finished.");
			}
		};
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

		// start the market data broadcasting
		if (marketDataMessages.empty())
		{
			LOGERR("No market data loaded.");
		}

		LOGINFO("Starting market data replay in {} seconds...", LEAD_SLEEP_SECONDS);
		scheduler.schedule(DispatchKey::MARKET_DATA, publishingTask, std::chrono::seconds(LEAD_SLEEP_SECONDS));

		return true;
	}

	void SimulationExchange::stop()
	{
		OrderManager<SimulationExchange, NullObjectPool>::stop();
	}

	bool SimulationExchange::subscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer)
	{
		if (UNLIKELY(lotSizeMap.find(subscription.key) == lotSizeMap.end()))
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
		if (UNLIKELY(lotSizeMap.find(subscription.key) == lotSizeMap.end()))
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
		if (UNLIKELY(lotSizeMap.find(subscription.key) == lotSizeMap.end()))
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
		if (UNLIKELY(lotSizeMap.find(subscription.key) == lotSizeMap.end()))
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

		symbolMap[product.symbol] = product.id;
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

	void SimulationExchange::loadMarketData(std::istream& is)
	{
		try
		{
			bool headerLoaded = false;
			std::chrono::microseconds lastTimestamp;

			std::vector<std::string> items;
			for (std::string line; std::getline(is, line); )
			{
				StringUtil::trim(line);
				if (line.size() == 0 || line[0] == '#')
				{
					continue;
				}

				if (!headerLoaded)
				{
					headerLoaded = true;
					continue;
				}

				boost::split(items, line, boost::is_any_of("|"));
				for (std::string& item : items)
				{
					StringUtil::trim(item);
				}

				std::shared_ptr<MarketDataMessage> marketData = marketDataPool.getShared();

				// sanity check
				SymbolType symbol = items[1];
				if (symbolMap.find(symbol) == symbolMap.end())
				{
					LOGWARN("Skipping unsupported instrument {}", items[1]);
				}

				// the timestamp
				const std::chrono::microseconds timestamp = std::chrono::microseconds(
						static_cast<std::int64_t> (std::stod(items[2]) * 1000 * 1000));

				// the market data
				marketData->instrumentId = symbolMap[symbol];
				marketData->last = std::stod(items[3]);
				marketData->volume = std::stol(items[4]);
				marketData->levels[toValue(Side::BID)][0].price = std::stod(items[5]);
				marketData->levels[toValue(Side::BID)][0].qty = std::stod(items[6]);
				marketData->levels[toValue(Side::ASK)][0].price = std::stod(items[7]);
				marketData->levels[toValue(Side::ASK)][0].qty = std::stod(items[8]);

				marketDataMessages.push_back(std::make_pair(timestamp, marketData));
			}
		}
		catch (std::exception& e)
		{
			LOGERR("Failed to read market data: {}", e.what());
		}
		catch (...)
		{
			LOGERR("Failed to read market data with unknown reason.");
		}
	}

	void SimulationExchange::determineExecution(const std::shared_ptr<MarketDataMessage>& marketData)
	{
		std::unordered_map<std::int64_t, OrderSummaryMessage>& orderMap = instrumentOrderMap[marketData->instrumentId];
		for (auto it = orderMap.begin(); it != orderMap.end(); )
		{
			OrderSummaryMessage& summary = it->second;

			// work out execution if any
			if ((summary.side == Side::BID && summary.price >= marketData->levels[toValue(Side::ASK)][0].price) ||
				(summary.side == Side::ASK && summary.price <= marketData->levels[toValue(Side::BID)][0].price))
			{
				std::int64_t execQty = 0;
				double execNotional = 0.0;
				const std::size_t index = summary.side == Side::BID ? toValue(Side::ASK) : toValue(Side::BID);

				for (std::size_t i = 0; i < MarketDataMessage::MAX_DEPTH && summary.remainQty() - execQty > 0; ++i)
				{
					if ((summary.side == Side::BID && summary.price >= marketData->levels[index][i].price) ||
						(summary.side == Side::ASK && summary.price <= marketData->levels[index][i].price))
					{
						const std::int64_t levelQty = std::min(summary.remainQty(), marketData->levels[index][i].qty);

						execQty += levelQty;
						execNotional += levelQty * marketData->levels[index][i].price;
					}
				}

				// publish the execution
				if (execQty > 0)
				{
					summary.tradedQty += execQty;
					summary.tradedNotional += execNotional;

					std::shared_ptr<ExecutionReportMessage> trade = executionReportPool.getShared();
					trade->instrumentId = summary.instrumentId;
					trade->orderId = summary.orderId;
					trade->price = summary.price;
					trade->side = summary.side;
					trade->execQty = execQty;
					trade->execPrice = execNotional / execQty;
					trade->executionId = fmt::format_int(++executionIdCounter).c_str();
					trade->tradeTimestamp = DateTimeUtil::now();
					trade->openQty = summary.remainQty();
					trade->tradedQty = summary.tradedQty;
					trade->status = summary.status;

					const Subscription subscription(SourceType::ALL, DataType::EXEC_REPORT, summary.strategyId);
					PublisherAdapter<ExecutionReportMessage>::publish(subscription, trade);
				}
			}

			if (OrderStatusUtil::completed(summary.status))
			{
				std::shared_ptr<ExecutionReportMessage> response = executionReportPool.getShared();
				response->instrumentId = summary.instrumentId;
				response->orderId = summary.orderId;
				response->price = summary.price;
				response->side = summary.side;
				response->openQty = 0;
				response->tradedQty = summary.tradedQty;
				response->status = summary.status;

				const Subscription subscription(SourceType::ALL, DataType::EXEC_REPORT, summary.strategyId);
				PublisherAdapter<ExecutionReportMessage>::publish(subscription, response);

				it = orderMap.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

}
