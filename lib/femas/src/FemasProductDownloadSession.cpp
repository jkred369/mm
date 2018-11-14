/*
 * FemasProductDownloadSession.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: suoalex
 */

#include <algorithm>
#include <array>
#include <fstream>

#include <EnumType.hpp>
#include <NativeDefinition.hpp>
#include <ProductService.hpp>
#include <StringBuffer.hpp>
#include <StringUtil.hpp>

#include "FemasProductDownloadSession.hpp"
#include "FemasUtil.hpp"

mm::Logger mm::FemasProductDownloadSession::logger;

namespace mm
{
	FemasProductDownloadSession::FemasProductDownloadSession(
			const KeyType dispatchKey,
			const std::string serviceName,
			ServiceContext& serviceContext,
			const std::string productServiceName,
			const std::string outputPath,
			const FemasUserDetail& userDetail,
			const std::unordered_set<ProductType> productTypes) :
		DispatchableService(dispatchKey, serviceName, serviceContext),
		outputPath(outputPath),
		userDetail(userDetail),
		productTypes(productTypes),
		dispatcher(serviceContext.getDispatcher()),
		session(CUstpFtdcTraderApi::CreateFtdcTraderApi()),
		productService(nullptr),
		stopFlag(false),
		requestId(0),
		maxProductId(0)
	{
		// Seeking product service.
		if (!serviceContext.getService(productServiceName, productService))
		{
			LOGERR("Product download session failed to get product service from service context");
			throw std::runtime_error("Failed to create Femas product download session. Cannot find product service with name: " + productServiceName);
		}

		productService->initSnapshot(this);

		// basic wiring up - the API interface forced const_cast usage
		session->RegisterSpi(this);

		if (!userDetail.frontAddress.empty())
		{
			session->RegisterFront(const_cast<char*> (userDetail.frontAddress.c_str()));
		}
		else if (!userDetail.nameServer.empty())
		{
			session->RegisterNameServer(const_cast<char*> (userDetail.nameServer.c_str()));
		}
		else
		{
			throw std::invalid_argument("No front address and name server.");
		}

		if (!userDetail.certFileName.empty())
		{
			session->RegisterCertificateFile(
					userDetail.certFileName.c_str(),
					userDetail.keyFileName.c_str(),
					userDetail.caFileName.c_str(),
					userDetail.keyFilePassword.c_str());
		}

		session->Init();
	}

	FemasProductDownloadSession::~FemasProductDownloadSession()
	{
		stop();
	}

	bool FemasProductDownloadSession::start()
	{
		if (!DispatchableService::start())
		{
			return false;
		}

		// wait on init latch to make sure the connection is established
		LOGINFO("Waiting for connection...");
		initLatch.wait();

		// login attempt
		const int result = login();
		if (result != 0)
		{
			LOGERR("Error loging as user {} with code: {}", userDetail.userId, result);
			return false;
		}

		LOGINFO("Waiting for login response...");
		startLatch.wait();

		// querying all the products
		{
			CUstpFtdcQryInstrumentField field;
			std::memset(&field, 0, sizeof(field));

			session->ReqQryInstrument(&field, ++requestId);
			outputLatch.wait();
		}

		// when output is finished we can always return false to cause the context shut down gracefully.
		return false;
	}

	void FemasProductDownloadSession::stop()
	{
		// sanity check
		{
			bool expected = false;
			if (!stopFlag.compare_exchange_strong(expected, true))
			{
				return;
			}
		}

		// attempt to logout but stop even if failed
		const int result = logout();
		if (result != 0)
		{
			LOGERR("Error logout user: {} with code: {}", userDetail.userId, result);
		}

		// actually stop the service
		// it's commented that session->join() isn't stable?
		session->RegisterSpi(nullptr);
		session->Release();
		session = nullptr;

		// delegate
		DispatchableService::stop();
	}

	void FemasProductDownloadSession::consume(const std::shared_ptr<const Product>& message)
	{
		const ProductMessage& content = message->getContent();
		symbolMap[content.symbol] = content.id;

		if (maxProductId < content.id)
		{
			maxProductId = content.id;
		}

		rawProductMap[content.id] = content;
	}

	void FemasProductDownloadSession::OnFrontConnected()
	{
		initLatch.countDown();
		LOGDEBUG("Product download session connected.");
	}

	void FemasProductDownloadSession::OnFrontDisconnected(int reason)
	{
		LOGDEBUG("Product download session disconnected on {}, auto-reconnecting", reason);
	}

	void FemasProductDownloadSession::OnHeartBeatWarning(int timeLapse)
	{
		LOGWARN("No heart beat in {} seconds.", timeLapse);
	}

	void FemasProductDownloadSession::OnPackageStart(int topicID, int sequenceNo)
	{
		LOGTRACE("Received packet {} on topic {}", sequenceNo, topicID);
	}

	void FemasProductDownloadSession::OnPackageEnd(int topicID, int sequenceNo)
	{
		LOGTRACE("Finished packet {} on topic {}", sequenceNo, topicID);
	}

	void FemasProductDownloadSession::OnRspError(CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		LOGERR("Error on request {} : {}, {}", requestID, info->ErrorID, info->ErrorMsg);
	}

	void FemasProductDownloadSession::OnRspUserLogin(CUstpFtdcRspUserLoginField *userLogin, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			tradingDate = userLogin->TradingDay;
			LOGINFO("User {} logged in successfully on date {}", userLogin->UserID, tradingDate);

			startLatch.countDown();
		}
		else
		{
			LOGERR("Login attempt failed. User: {}, error: {}, {}", userLogin->UserID, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasProductDownloadSession::OnRspUserLogout(CUstpFtdcRspUserLogoutField *userLogout, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("User {} logged out successfully", userLogout->UserID);
		}
		else
		{
			LOGERR("Logout attempt failed. User: {}, error: {}, {}", userLogout->UserID, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasProductDownloadSession::OnRspSubscribeTopic(CUstpFtdcDisseminationField *dissemination, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("Subscribed to {}", dissemination->SequenceSeries);
		}
		else
		{
			LOGERR("Error subscribing to {}: {}, {}", dissemination->SequenceSeries, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasProductDownloadSession::OnRspQryTopic(CUstpFtdcDisseminationField *dissemination, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("Queried topic {}", dissemination->SequenceSeries);
		}
		else
		{
			LOGERR("Error querying {}: {}, {}", dissemination->SequenceSeries, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasProductDownloadSession::OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *userPasswordUpdate, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("User {} password updated from {} to {}", userPasswordUpdate->UserID, userPasswordUpdate->OldPassword, userPasswordUpdate->NewPassword);
		}
		else
		{
			LOGERR("User {} password update failed: {}, {}", userPasswordUpdate->UserID, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasProductDownloadSession::OnRspOrderInsert(CUstpFtdcInputOrderField *inputOrder, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasProductDownloadSession::OnRspOrderAction(CUstpFtdcOrderActionField *orderAction, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasProductDownloadSession::OnRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *flowMessageCancel)
	{

	}

	void FemasProductDownloadSession::OnRtnTrade(CUstpFtdcTradeField *trade)
	{

	}

	void FemasProductDownloadSession::OnRtnOrder(CUstpFtdcOrderField *order)
	{

	}

	void FemasProductDownloadSession::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *inputOrder, CUstpFtdcRspInfoField *info)
	{

	}

	void FemasProductDownloadSession::OnErrRtnOrderAction(CUstpFtdcOrderActionField *orderAction, CUstpFtdcRspInfoField *info)
	{

	}

	void FemasProductDownloadSession::OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *instrumentStatus)
	{

	}

	void FemasProductDownloadSession::OnRtnInvestorAccountDeposit(CUstpFtdcInvestorAccountDepositResField *investorAccountDepositRes)
	{

	}

	void FemasProductDownloadSession::OnRspQryOrder(CUstpFtdcOrderField *order, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasProductDownloadSession::OnRspQryTrade(CUstpFtdcTradeField *trade, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasProductDownloadSession::OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *userInvestor, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
	}

	void FemasProductDownloadSession::OnRspQryTradingCode(CUstpFtdcRspTradingCodeField *tradingCode, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
	}

	void FemasProductDownloadSession::OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *investorAccount, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasProductDownloadSession::OnRspQryInstrument(CUstpFtdcRspInstrumentField *instrument, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		instruments.push_back(*instrument);

		// proceed the final product writing
		if (isLast)
		{
			LOGINFO("All instruments retrieved. Updating...");
			buildProduct();

			LOGINFO("All products built. Outputing...");
			output();
		}
	}

	void FemasProductDownloadSession::OnRspQryExchange(CUstpFtdcRspExchangeField *exchange, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			const std::string femasExchangeId = exchange->ExchangeID;

			dispatcher.submit(dispatchKey, [this, femasExchangeId] () {
				exchangeId = femasExchangeId;
				LOGINFO("Exchange ID set to: {}", exchangeId);

				startLatch.countDown();
			});
		}
		else
		{
			LOGERR("Error getting exchange ID with error: {}, {}", info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasProductDownloadSession::OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *investorPosition, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasProductDownloadSession::OnRspQryComplianceParam(CUstpFtdcRspComplianceParamField *complianceParam, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasProductDownloadSession::OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *investorFee, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasProductDownloadSession::OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *investorMargin, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	int FemasProductDownloadSession::login()
	{
		CUstpFtdcReqUserLoginField field;
		std::memset(&field, 0, sizeof(field));

		field.DataCenterID = userDetail.dataCenterId;

		StringUtil::copy(field.BrokerID, userDetail.brokerId, sizeof(field.BrokerID));
		StringUtil::copy(field.IPAddress, userDetail.ipAddress, sizeof(field.IPAddress));
		StringUtil::copy(field.InterfaceProductInfo, userDetail.interfaceProductInfo, sizeof(field.InterfaceProductInfo));
		StringUtil::copy(field.MacAddress, userDetail.macAddress, sizeof(field.MacAddress));
		StringUtil::copy(field.Password, userDetail.password, sizeof(field.Password));
		StringUtil::copy(field.ProtocolInfo, userDetail.protocolInfo, sizeof(field.ProtocolInfo));
		StringUtil::copy(field.TradingDay, session->GetTradingDay(), sizeof(field.TradingDay));
		StringUtil::copy(field.UserID, userDetail.userId, sizeof(field.UserID));
		StringUtil::copy(field.UserProductInfo, userDetail.userProductInfo, sizeof(field.UserProductInfo));

		const int result = session->ReqUserLogin(&field, ++requestId);
		return result;
	}

	int FemasProductDownloadSession::logout()
	{
		CUstpFtdcReqUserLogoutField field;
		StringUtil::copy(field.BrokerID, userDetail.brokerId, sizeof(field.BrokerID));
		StringUtil::copy(field.UserID, userDetail.userId, sizeof(field.UserID));

		const int result = session->ReqUserLogout(&field, ++requestId);
		return result;
	}

	ProductType FemasProductDownloadSession::getProductType(const CUstpFtdcRspInstrumentField& field) const
	{
		// TODO: Determine the product types.
		if (field.OptionsType != USTP_FTDC_OT_NotOptions)
		{
			return ProductType::EUROPEAN;
		}

		return ProductType::FUTURE;
	}

	void FemasProductDownloadSession::buildProduct()
	{
		std::unordered_set<SymbolType> processedSymbols;

		while (true)
		{
			const std::size_t prevSize = processedSymbols.size();

			for (const CUstpFtdcRspInstrumentField& instrument : instruments)
			{
				ProductMessage product;

				// check on symbol
				const SymbolType symbol(instrument.InstrumentID);
				if (processedSymbols.find(symbol) != processedSymbols.end())
				{
					continue;
				}

				// check on product type
				const ProductType productType = getProductType(instrument);
				if (!productTypes.empty() && productTypes.find(productType) == productTypes.end())
				{
					LOGINFO("Skip product {} with undesired type: {} ", symbol.toString(), toValue(productType));
					continue;
				}

				// determine product ID
				{
					auto it = symbolMap.find(symbol);

					if (it == symbolMap.end())
					{
						product.id = ++maxProductId;
						LOGINFO("New product {} ID set to {}", std::string(instrument.InstrumentID), product.id);
					}
					else
					{
						product.id = it->second;
					}
				}

				// determine underlying ID
				bool underlyingFound = false;
				{
					const SymbolType underlyingSymbol(instrument.UnderlyingInstrID);
					if (underlyingSymbol.stringSize() > 0)
					{
						auto it = symbolMap.find(underlyingSymbol);
						if (it == symbolMap.end())
						{
							LOGERR("Failed to find underlying with symbol: {}", underlyingSymbol.toString());
						}
						else
						{
							product.underlyerId = it->second;
							underlyingFound = true;
						}
					}
				}

				// fill in all the fields
				if (underlyingFound)
				{
					product.symbol = instrument.InstrumentID;
					product.productType = productType;
					product.currency = FemasUtil::getCurrency(instrument.Currency);
					product.exchange = FemasUtil::getExchange(instrument.ExchangeID);

					product.contractRatio = instrument.UnderlyingMultiple;
					product.lotSize = instrument.VolumeMultiple;
					product.listingDate = FemasUtil::getDate(instrument.OpenDate);
					product.lastTradingDate = FemasUtil::getDate(instrument.ExpireDate);
					product.expiryDate = FemasUtil::getDate(instrument.ExpireDate);

					if (instrument.OptionsType != USTP_FTDC_OT_NotOptions)
					{
						product.callPut = instrument.OptionsType ==
								USTP_FTDC_OT_CallOptions ? CallPutType::CALL : CallPutType::PUT;
						product.strike = instrument.StrikePrice;
					}

					// add the product to the map for writing out
					rawProductMap[product.id] = product;
					processedSymbols.insert(symbol);
					LOGINFO("Refreshed product {}, {}", product.id, product.symbol.toString());
				}
			}

			// if we cannot make any progress, let it be
			if (prevSize == processedSymbols.size())
			{
				break;
			}
		}

		LOGINFO("{} products added or updated.", processedSymbols.size());
	}

	void FemasProductDownloadSession::output()
	{
		std::vector<std::int64_t> ids;
		ids.reserve(rawProductMap.size());

		// firstly, remove all the products that are expired
		const Date today = DateUtil::today();
		for (auto it = rawProductMap.begin(); it != rawProductMap.end(); )
		{
			const ProductMessage& product = it->second;
			if (product.expiryDate < today)
			{
				it = rawProductMap.erase(it);
			}
			else
			{
				ids.push_back(it->first);
				++it;
			}
		}

		std::sort(ids.begin(), ids.end());
		LOGINFO("{} products will be written to {}", rawProductMap.size(), outputPath);

		// output
		{
			std::ofstream os(outputPath);
			for (const std::int64_t& id : ids)
			{
				StringBuffer buffer;

				const ProductMessage& product = rawProductMap[id];
				product.serialize(buffer);
				os << buffer;
			}

			os.flush();
			LOGINFO("Product output finished.");

			outputLatch.countDown();
		}
	}

}



