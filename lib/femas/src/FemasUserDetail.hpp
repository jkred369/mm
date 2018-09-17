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

namespace mm
{
	//
	// This class defines the user login detail fields.
	//
	struct FemasUserDetail
	{
		FemasUserDetail()
		{
		}

		FemasUserDetail(
				const std::string& frontAddress,
				const std::string& nameServer,
				const std::string& certFileName,
				const std::string& keyFileName,
				const std::string& caFileName,
				const std::string& keyFilePassword,
				const std::string& userId,
				const std::string& dataCenterId,
				const std::string& brokerId,
				const std::string& tradingSystemName,
				const std::int64_t maxOrderLocalId,
				const std::int64_t privteFlowSize,
				const std::int64_t userFlowSize) :
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
			maxOrderLocalId(maxOrderLocalId),
			privateFlowSize(privateFlowSize),
			userFlowSize(userFlowSize)
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
		std::string dataCenterId;

		// Broker ID.
		std::string brokerId;

		// Trading system name.
		std::string tradingSystemName;

		// Max order local ID.
		std::int64_t maxOrderLocalId;

		// Private flow size.
		std::int64_t privateFlowSize;

		// User flow size.
		std::int64_t userFlowSize;
	};
}



#endif /* LIB_FEMAS_SRC_FEMASUSERDETAIL_HPP_ */
