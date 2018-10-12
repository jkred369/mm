/*
 * FemasUserDetail.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: suoalex
 */

#include "FemasUserDetail.hpp"

const std::string mm::FemasUserDetailConfig::FRONT_ADDRESS = "FrontAddress";
const std::string mm::FemasUserDetailConfig::NAME_SERVER = "NameServer";
const std::string mm::FemasUserDetailConfig::CERT_FILE_NAME = "CertFileName";
const std::string mm::FemasUserDetailConfig::KEY_FILE_NAME = "KeyFileName";
const std::string mm::FemasUserDetailConfig::CA_FILE_NAME = "CaFileName";
const std::string mm::FemasUserDetailConfig::KEY_FILE_PASSWORD = "KeyFilePassword";
const std::string mm::FemasUserDetailConfig::USER_ID = "UserId";
const std::string mm::FemasUserDetailConfig::DATA_CENTER_ID = "DataCenterId";
const std::string mm::FemasUserDetailConfig::BROKER_ID = "BrokerId";
const std::string mm::FemasUserDetailConfig::TRADING_SYSTEM_NAME = "TradingSystemName";
const std::string mm::FemasUserDetailConfig::MAC_ADDRESS = "MacAddress";
const std::string mm::FemasUserDetailConfig::IP_ADDRESS = "IpAddress";
const std::string mm::FemasUserDetailConfig::INTERFACE_PRODUCT_INFO = "InterfaceProductInfo";
const std::string mm::FemasUserDetailConfig::USER_PRODUCE_INFO = "UserProductInfo";
const std::string mm::FemasUserDetailConfig::PASSWORD = "Password";
const std::string mm::FemasUserDetailConfig::PROTOCOL_INFO = "ProtocolInfo";

namespace mm
{
	FemasUserDetail::FemasUserDetail(const std::shared_ptr<IConfig> config) :
			FemasUserDetail(
					config->getString(FemasUserDetailConfig::FRONT_ADDRESS, ""),
					config->getString(FemasUserDetailConfig::NAME_SERVER, ""),
					config->getString(FemasUserDetailConfig::CERT_FILE_NAME, ""),
					config->getString(FemasUserDetailConfig::KEY_FILE_NAME, ""),
					config->getString(FemasUserDetailConfig::CA_FILE_NAME, ""),
					config->getString(FemasUserDetailConfig::KEY_FILE_PASSWORD, ""),
					config->getString(FemasUserDetailConfig::USER_ID, ""),
					config->getInt64(FemasUserDetailConfig::DATA_CENTER_ID, 0),
					config->getString(FemasUserDetailConfig::BROKER_ID, ""),
					config->getString(FemasUserDetailConfig::TRADING_SYSTEM_NAME, ""),
					config->getString(FemasUserDetailConfig::MAC_ADDRESS, ""),
					config->getString(FemasUserDetailConfig::IP_ADDRESS, ""),
					config->getString(FemasUserDetailConfig::INTERFACE_PRODUCT_INFO, ""),
					config->getString(FemasUserDetailConfig::USER_PRODUCE_INFO, ""),
					config->getString(FemasUserDetailConfig::PASSWORD, ""),
					config->getString(FemasUserDetailConfig::PROTOCOL_INFO, ""))
	{
	}
}
