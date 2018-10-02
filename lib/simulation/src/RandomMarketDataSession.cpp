/*
 * RandomMarketDataSession.cpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#include <chrono>

#include <ProductService.hpp>

#include "RandomMarketDataSession.hpp"

mm::Logger mm::RandomMarketDataSession::logger;

namespace mm
{
	RandomMarketDataSession::RandomMarketDataSession(KeyType dispatchKey, ServiceContext& serviceContext, const std::string& productServiceName) :
		PublisherAdapter<MarketDataMessage>(dispatchKey, serviceContext.getDispatcher()),
		scheduler(serviceContext.getScheduler()),
		pool(100),
		distribution(-1, 1)
	{
		static_assert(BID >= 0, "Bid index must be positive.");
		static_assert(ASK >= 0, "Ask index must be positive.");

		// create symbol mapping
		{
			ProductService* productService;
			if (!serviceContext.getService(productServiceName, productService))
			{
				LOGERR("Market data session failed to get product service from service context");
				throw std::runtime_error("Failed to create Femas market data session. Cannot find product service with name: " + productServiceName);
			}

			productService->initSnapshot(this);
			LOGINFO("ID set created with {} products.", supportedSet.size());
		}

		// random seed
		randSeed.seed(std::random_device()());
	}

	bool RandomMarketDataSession::start()
	{
		return true;
	}

	void RandomMarketDataSession::stop()
	{
	}

	bool RandomMarketDataSession::subscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer)
	{
		if (subscription.dataType != DataType::MARKET_DATA)
		{
			return false;
		}

		const std::int64_t id = subscription.key;
		if (supportedSet.find(id) == supportedSet.end())
		{
			LOGWARN("Attempt to subscribe to unsupported instrument: {}", id);
			return false;
		}

		if (requestedSet.find(id) != requestedSet.end())
		{
			// already publishing
			return true;
		}

		if (!PublisherAdapter<MarketDataMessage>::subscribe(subscription, consumer))
		{
			return false;
		}

		requestedSet.insert(id);

		// some tricks for the market data
		// assuming all tick size = 1
		const long initValue = std::abs(id) * 100 * distribution(randSeed);

		scheduler.scheduleAtFixedRate(DispatchKey::MARKET_DATA, [this, id, initValue] () {

			std::shared_ptr<MarketDataMessage> messagePtr = pool.getShared();
			MarketDataMessage& message = *messagePtr;

			message.instrumentId = id;

			message.open = initValue;
			message.close = initValue;
			message.high = initValue;
			message.low = initValue;
			message.last = std::round(initValue * (1 + distribution(randSeed)));
			message.highLimit = initValue * 2;
			message.lowLimit = std::round(initValue * 0.5);
			message.volume = 10000;
			message.turnover = message.last * message.volume;

			// market data levels - silly yes but fast
			for (int i = 0; i < 5; ++i)
			{
				message.levels[BID][i].price = std::max(message.last - i, 0.0);
				message.levels[BID][i].qty = 10000 - i * 1000;

				message.levels[ASK][i].price = message.last + i;
				message.levels[ASK][i].qty = 10000 - i * 1000;
			}

			const Subscription subscription = {SourceType::ALL, DataType::MARKET_DATA, id};
			this->publish(subscription, messagePtr);

		}, std::chrono::milliseconds(1000), std::chrono::milliseconds(100));

		LOGINFO("Simulating market data for {}", id);
		return true;
	}

	void RandomMarketDataSession::unsubscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer)
	{
		PublisherAdapter<MarketDataMessage>::unsubscribe(subscription, consumer);
	}

	void RandomMarketDataSession::consume(const std::shared_ptr<const Product>& message)
	{
		supportedSet.insert(message->getContent().id);
	}

}
