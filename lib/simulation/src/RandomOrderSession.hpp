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

#include <EnumType.hpp>
#include <IConsumer.hpp>
#include <IService.hpp>
#include <Logger.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>
#include <Product.hpp>
#include <PublisherAdapter.hpp>
#include <ServiceContext.hpp>

namespace mm
{
	//
	// This class defines a random order session for simulation, modeling ExchangeInterface concept.
	//
	class RandomOrderSession :
		public IService,
		public IConsumer<Product>
	{
	public:

		//
		// Constructor.
		//
		// serviceContext : The service context.
		// productServiceName : The name of the product service.
		//
		RandomOrderSession(
				ServiceContext& serviceContext,
				const std::string& productServiceName);

		// virtual destructor.
		virtual ~RandomOrderSession();

		//
		// service interface.
		//
		virtual bool start() override;
		virtual void stop() override;

		//
		// Send new order to exchange.
		//
		// message : The message representing new order.
		//
		void sendOrder(const std::shared_ptr<OrderMessage>& message);

		//
		// Cancel order.
		//
		// message : The message representing order to cancel.
		//
		void cancel(const std::shared_ptr<OrderMessage>& message);

	private:

		inline std::shared_ptr<OrderSummaryMessage> getMessage()
		{
			return std::make_shared<OrderSummaryMessage>();
		}

		//
		// Get the exchange order ID from the client order ID.
		//
		// clientOrderId : The client order ID.
		//
		// return : The exchange order ID type.
		//
		inline bool getExchangeOrderId(const std::int64_t clientOrderId, TUstpFtdcOrderSysIDType exchangeOrderId)
		{
			auto it = orderIdMap.find(clientOrderId);
			if (LIKELY(it != orderIdMap.end()))
			{
				std::memcpy(exchangeOrderId, it->second, sizeof(TUstpFtdcOrderSysIDType));
				return true;
			}
			else
			{
				LOGERR("Failed to get exchange order ID for order ID: {}", clientOrderId);
				return false;
			}
		}

		// The int value for bid
		static constexpr int BID = toValue(Side::BID);

		// The int value for ask
		static constexpr int ASK = toValue(Side::ASK);

		// The logger for this class.
		static Logger logger;

		// The map of the live order's client order ID.
		std::unordered_set<std::int64_t> liveOrderSet;
	};
}

#endif /* LIB_SIMULATION_SRC_RANDOMORDERSESSION_HPP_ */
