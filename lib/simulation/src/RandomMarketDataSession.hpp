/*
 * RandomMarketDataSession.hpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#ifndef LIB_SIMULATION_SRC_RANDOMMARKETDATASESSION_HPP_
#define LIB_SIMULATION_SRC_RANDOMMARKETDATASESSION_HPP_

#include <atomic>
#include <memory>
#include <random>
#include <string>
#include <unordered_set>

#include <EnumType.hpp>
#include <FieldDefinition.hpp>
#include <IConsumer.hpp>
#include <IService.hpp>
#include <Logger.hpp>
#include <MarketDataMessage.hpp>
#include <NativeDefinition.hpp>
#include <NullObjectPool.hpp>
#include <Product.hpp>
#include <PublisherAdapter.hpp>
#include <ServiceContext.hpp>
#include <Scheduler.hpp>

namespace mm
{
	//
	// This class defines a randomized market data session used for testing.
	//
	class RandomMarketDataSession :
			public IService,
			public PublisherAdapter<MarketDataMessage>,
			public IConsumer<Product>
	{
	public:

		//
		// Constructor.
		//
		// dispatchKey : The dispatch key.
		// serviceContext : The service context.
		// productServiceName : The name of the product service.
		//
		RandomMarketDataSession(
				KeyType dispatchKey,
				ServiceContext& serviceContext,
				const std::string& productServiceName);

		//
		// Destructor.
		//
		virtual ~RandomMarketDataSession();

		//
		// service interface.
		//
		virtual bool start() override;
		virtual void stop() override;

		//
		// publisher functionality.
		//
		virtual const KeyType getKey() const override;
		virtual bool subscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer) override;
		virtual void unsubscribe(const Subscription& subscription, IConsumer<MarketDataMessage>* consumer) override;

		//
		// consumer functionality.
		//
		virtual void consume(const std::shared_ptr<const Product>& message) override;

	private:

		// The int value for bid
		static constexpr int BID = toValue(Side::BID);

		// The int value for ask
		static constexpr int ASK = toValue(Side::ASK);

		// The logger for this class.
		static Logger logger;

		// The dispatch key.
		const KeyType dispatchKey;

		// The scheduler.
		Scheduler& scheduler;

		// The pool used.
		NullObjectPool<MarketDataMessage> pool;

		// The random seed.
		std::mt19937 randSeed;

		// The randome distribution.
		std::uniform_real_distribution<> distribution;

		// The supported instrument IDs.
		std::unordered_set<std::int64_t> supportedSet;

		// The requested IDs (in publish IDs)
		std::unordered_set<std::int64_t> requestedSet;

	};
}

#endif /* LIB_SIMULATION_SRC_RANDOMMARKETDATASESSION_HPP_ */
