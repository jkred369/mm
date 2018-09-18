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
#include <SubscriberAdapter.hpp>

#include <FemasUserDetail.hpp>

namespace mm
{
	class FemasOrderSession :
			public IService,
			public CUstpFtdcTraderSpi,
			public SubscriberAdapter<OrderMessage>
	{
	};
}



#endif /* LIB_FEMAS_SRC_FEMASORDERSESSION_HPP_ */
