/*
 * FemasOrderSession.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_FEMAS_SRC_FEMASORDERSESSION_HPP_
#define LIB_FEMAS_SRC_FEMASORDERSESSION_HPP_

#include <atomic>
#include <memory>
#include <string>

#include <femas/USTPFtdcTraderApi.h>

#include <Logger.hpp>
#include <EnumType.hpp>
#include <IService.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>
#include <SubscriberAdapter.hpp>

#include <FemasUserDetail.hpp>

namespace mm
{
	//
	// This class defines an order session.
	//
	// An order session subcribes to order message for order status manipulation and publishes order summary.
	//
	class FemasOrderSession :
			public IService,
			public CUstpFtdcTraderSpi,
			public IConsumer<OrderMessage>,
			public SubscriberAdapter<OrderSummaryMessage>
	{
	public:

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

	};
}



#endif /* LIB_FEMAS_SRC_FEMASORDERSESSION_HPP_ */
