/*
 * RandomOrderSession.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: suoalex
 */

#include <ProductService.hpp>

#include "RandomOrderSession.hpp"

mm::Logger mm::RandomOrderSession::logger;

namespace mm
{
	RandomOrderSession::RandomOrderSession(
			const KeyType dispatchKey,
			const std::string serviceName,
			ServiceContext& serviceContext,
			const std::string productServiceName) :
		OrderManager<RandomOrderSession, NullObjectPool>(dispatchKey, serviceName, serviceContext, *this),
		scheduler(serviceContext.getScheduler()),
		executionReportPool(1000)
	{
		static_assert(BID >= 0, "Bid index must be positive.");
		static_assert(ASK >= 0, "Ask index must be positive.");

		// create symbol mapping
		{
			ProductService* productService;
			if (!serviceContext.getService(productServiceName, productService))
			{
				LOGERR("Order session failed to get product service from service context");
				throw std::runtime_error("Failed to create Random order session. Cannot find product service with name: " + productServiceName);
			}

			productService->initSnapshot(this);
			LOGINFO("ID set created with {} products.", supportedSet.size());
		}
	}

	RandomOrderSession::~RandomOrderSession()
	{
	}

	void RandomOrderSession::consume(const std::shared_ptr<const Product>& message)
	{
		supportedSet.insert(message->getContent().id);
	}

	void RandomOrderSession::sendOrder(const std::shared_ptr<const OrderMessage>& message)
	{
		if (liveOrderMap.find(message->orderId) != liveOrderMap.end())
		{
			LOGERR("Failed to operate on order with id: {}, requested status: {}", message->orderId, toValue(message->status));
			return;
		}

		const OrderMessage copy = *message;

		scheduler.schedule(getKey(), [this, copy] () {

			const std::shared_ptr<ExecutionReportMessage> reportPtr = executionReportPool.getShared();
			ExecutionReportMessage& report = *reportPtr;

			report.orderId = copy.orderId;
			report.instrumentId = copy.instrumentId;
			report.openQty = copy.totalQty;
			report.price = copy.price;
			report.side = copy.side;
			report.status = OrderStatus::LIVE;

			OrderManager<RandomOrderSession, NullObjectPool>::consume(reportPtr);

		}, std::chrono::milliseconds(2));
	}

	void RandomOrderSession::cancel(const std::shared_ptr<const OrderMessage>& message)
	{
		if (liveOrderMap.find(message->orderId) == liveOrderMap.end())
		{
			LOGERR("Failed to find order with id: {}, requested status: {}", message->orderId, toValue(message->status));
			return;
		}

		const OrderMessage copy = *message;

		scheduler.schedule(getKey(), [this, copy] () {

			const std::shared_ptr<ExecutionReportMessage> reportPtr = executionReportPool.getShared();
			ExecutionReportMessage& report = *reportPtr;

			report.orderId = copy.orderId;
			report.instrumentId = copy.instrumentId;
			report.status = OrderStatus::CANCELLED;

			OrderManager<RandomOrderSession, NullObjectPool>::consume(reportPtr);

		}, std::chrono::milliseconds(2));
	}

}
