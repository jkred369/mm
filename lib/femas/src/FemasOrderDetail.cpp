/*
 * FemasOrderDetail.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: suoalex
 */

#include "FemasOrderDetail.hpp"

const std::string mm::FemasOrderDetailConfig::HEDGE_FLAG = "HedgeFlag";
const std::string mm::FemasOrderDetailConfig::IS_AUTO_SUSPEND = "IsAutoSuspend";
const std::string mm::FemasOrderDetailConfig::EXCHANGE_ID = "ExchangeId";

namespace mm
{
	FemasOrderDetail::FemasOrderDetail(const std::shared_ptr<IConfig> config) :
			FemasOrderDetail(
					config->getInt64(FemasOrderDetailConfig::HEDGE_FLAG),
					config->getInt64(FemasOrderDetailConfig::IS_AUTO_SUSPEND, 0),
					config->getString(FemasOrderDetailConfig::EXCHANGE_ID))
	{
	}
}
