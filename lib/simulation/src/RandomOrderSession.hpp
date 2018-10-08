/*
 * RandomOrderSession.hpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#ifndef LIB_SIMULATION_SRC_RANDOMORDERSESSION_HPP_
#define LIB_SIMULATION_SRC_RANDOMORDERSESSION_HPP_

#include <atomic>
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include <EnumType.hpp>
#include <IConsumer.hpp>
#include <IService.hpp>
#include <Logger.hpp>
#include <NullObjectPool.hpp>
#include <OrderManager.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>
#include <OrderStatus.hpp>
#include <Product.hpp>
#include <PublisherAdapter.hpp>
#include <ServiceContext.hpp>

namespace mm
{
	//
	// This class defines a random order session for simulation, modeling ExchangeInterface concept.
	//
	class RandomOrderSession :
		public OrderManager<RandomOrderSession, NullObjectPool>,
		public IConsumer<Product>
	{
	public:

		//
		// Constructor.
		//
		// dispatchKey : dispatch key for the session.
		// servieName : The service name.
		// serviceContext : The service context.
		// productServiceName : The name of the product service.
		//
		RandomOrderSession(
				const KeyType dispatchKey,
				const std::string serviceName,
				ServiceContext& serviceContext,
				const std::string productServiceName);

		// virtual destructor.
		virtual ~RandomOrderSession();

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

		// The logger for this class.
		static Logger logger;

		// The scheduler used.
		Scheduler& scheduler;

		// The pool for execution report messages.
		NullObjectPool<ExecutionReportMessage> executionReportPool;

		// The supported instrument IDs.
		std::unordered_set<std::int64_t> supportedSet;

		// The map of the live order's client order ID.
		std::unordered_map<std::int64_t, OrderStatus> liveOrderMap;
	};
}

#endif /* LIB_SIMULATION_SRC_RANDOMORDERSESSION_HPP_ */
