/*
 * FemasUserDetail.hpp
 *
 *  Created on: Sep 17, 2018
 *      Author: suoalex
 */

#ifndef LIB_FEMAS_SRC_FEMASUSERDETAIL_HPP_
#define LIB_FEMAS_SRC_FEMASUSERDETAIL_HPP_

#include <cstdint>
#include <string>

#include <IConfig.hpp>

namespace mm
{
	//
	// The config keys for user detail object.
	//
	struct FemasUserDetailConfig
	{
		// The front address.
		static const std::string FRONT_ADDRESS;

		// The name serveri.
		static const std::string NAME_SERVER;

		// The cert file name.
		static const std::string CERT_FILE_NAME;

		// The key file name.
		static const std::string KEY_FILE_NAME;

		// The CA file name.
		static const std::string CA_FILE_NAME;

		// The key file password.
		static const std::string KEY_FILE_PASSWORD;

		// The user ID.
		static const std::string USER_ID;

		// The data center ID.
		static const std::string DATA_CENTER_ID;

		// The broker ID.
		static const std::string BROKER_ID;

		// The trading system name.
		static const std::string TRADING_SYSTEM_NAME;

		// The mac address.
		static const std::string MAC_ADDRESS;

		// The IP address.
		static const std::string IP_ADDRESS;

		// The interface info.
		static const std::string INTERFACE_PRODUCT_INFO;

		// The user product info.
		static const std::string USER_PRODUCE_INFO;

		// The password.
		static const std::string PASSWORD;

		// The protocol info.
		static const std::string PROTOCOL_INFO;
	};

	//
	// This class defines the user login detail fields.
	//
	struct FemasUserDetail
	{
		FemasUserDetail()
		{
		}

		//
		// retrieve the fields from config.
		//
		// config : The config object.
		//
		FemasUserDetail(const std::shared_ptr<IConfig> config);

		FemasUserDetail(
				const std::string& frontAddress,
				const std::string& nameServer,
				const std::string& certFileName,
				const std::string& keyFileName,
				const std::string& caFileName,
				const std::string& keyFilePassword,
				const std::string& userId,
				const std::int64_t dataCenterId,
				const std::string& brokerId,
				const std::string& tradingSystemName,
				const std::string& macAddress,
				const std::string& ipAddress,
				const std::string& interfaceProductInfo,
				const std::string& userProductInfo,
				const std::string& password,
				const std::string& protocolInfo) :
			frontAddress(frontAddress),
			nameServer(nameServer),
			certFileName(certFileName),
			keyFileName(keyFileName),
			caFileName(caFileName),
			keyFilePassword(keyFilePassword),
			userId(userId),
			dataCenterId(dataCenterId),
			brokerId(brokerId),
			tradingSystemName(tradingSystemName),
			macAddress(macAddress),
			ipAddress(ipAddress),
			interfaceProductInfo(interfaceProductInfo),
			userProductInfo(userProductInfo),
			password(password),
			protocolInfo(protocolInfo)
		{
		}

		// Front address
		std::string frontAddress;

		// Name server address.
		std::string nameServer;

		// Certificate file path.
		std::string certFileName;

		// Key file path.
		std::string keyFileName;

		// CA file path.
		std::string caFileName;

		// Password for key file.
		std::string keyFilePassword;

		// User ID.
		std::string userId;

		// Data center ID.
		std::int64_t dataCenterId;

		// Broker ID.
		std::string brokerId;

		// Trading system name.
		std::string tradingSystemName;

		// MAC address
		std::string macAddress;

		// IP address
		std::string ipAddress;

		// interface product info.
		std::string interfaceProductInfo;

		// user product info.
		std::string userProductInfo;

		// Password.
		std::string password;

		// Protocol info.
		std::string protocolInfo;
	};
}



#endif /* LIB_FEMAS_SRC_FEMASUSERDETAIL_HPP_ */
