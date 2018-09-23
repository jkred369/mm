/*
 * FemasOrderSession.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: suoalex
 */

#include <fmt/format.h>

#include <StringUtil.hpp>
#include "FemasOrderSession.hpp"

mm::Logger mm::FemasOrderSession::logger;

namespace mm
{
	FemasOrderSession::FemasOrderSession(const FemasUserDetail& detail) :
		userDetail(detail),
		session(CUstpFtdcTraderApi::CreateFtdcTraderApi()),
		stopFlag(false),
		requestId(0)
	{
		static_assert(BID >= 0, "Bid index must be positive.");
		static_assert(ASK >= 0, "Ask index must be positive.");

		// basic wiring up - the API interface forced const_cast usage
		session->RegisterSpi(this);
		session->RegisterFront(const_cast<char*> (userDetail.frontAddress.c_str()));
		session->RegisterNameServer(const_cast<char*> (userDetail.nameServer.c_str()));

		session->RegisterCertificateFile(
				userDetail.certFileName.c_str(),
				userDetail.keyFileName.c_str(),
				userDetail.caFileName.c_str(),
				userDetail.keyFilePassword.c_str());

		session->Init();
	}

	FemasOrderSession::~FemasOrderSession()
	{
		stop();
	}

	bool FemasOrderSession::start()
	{
		// login attempt
		CUstpFtdcReqUserLoginField field;
		StringUtil::copy(field.BrokerID, userDetail.brokerId, sizeof(field.BrokerID));
		field.DataCenterID = userDetail.dataCenterId;

		StringUtil::copy(field.IPAddress, userDetail.ipAddress, sizeof(field.IPAddress));
		StringUtil::copy(field.InterfaceProductInfo, userDetail.interfaceProductInfo, sizeof(field.InterfaceProductInfo));
		StringUtil::copy(field.MacAddress, userDetail.macAddress, sizeof(field.MacAddress));
		StringUtil::copy(field.Password, userDetail.password, sizeof(field.Password));
		StringUtil::copy(field.ProtocolInfo, userDetail.protocolInfo, sizeof(field.ProtocolInfo));
		StringUtil::copy(field.TradingDay, session->GetTradingDay(), sizeof(field.TradingDay));
		StringUtil::copy(field.UserID, userDetail.userId, sizeof(field.UserID));
		StringUtil::copy(field.UserProductInfo, userDetail.userProductInfo, sizeof(field.UserProductInfo));

		const int result = session->ReqUserLogin(&field, ++requestId);

		if (result == 0)
		{
			LOGINFO("Market data session logged in as {}", userDetail.userId);
			return true;
		}
		else
		{
			LOGERR("Error loging as user {} with code: {}", userDetail.userId, result);
			return false;
		}
	}

	void FemasOrderSession::stop()
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
		{
			CUstpFtdcReqUserLogoutField field;
			StringUtil::copy(field.BrokerID, userDetail.brokerId, sizeof(field.BrokerID));
			StringUtil::copy(field.UserID, userDetail.userId, sizeof(field.UserID));

			const int result = session->ReqUserLogout(&field, ++requestId);
			if (result != 0)
			{
				LOGERR("Error logout user: {} with code: {}", userDetail.userId, result);
			}
		}

		// actually stop the service
		// it's commented that session->join() isn't stable?
		session->RegisterSpi(nullptr);
		session->Release();
		session = nullptr;
	}

	void FemasOrderSession::subscribe(const Subscription& subscription, const std::shared_ptr<IConsumer<OrderSummaryMessage> >& consumer)
	{
		PublisherAdapter<MarketDataMessage>::subscribe(subscription, consumer);

		fmt::format_int format(subscription.getKey());

		// femas requires char* without writing to it
		char* request[1] = {const_cast<char*> (format.c_str())};
		const int result = session->SubMarketData(request, 1);

		if (result != 0)
		{
			LOGERR("Error subscribing to {}", subscription.getKey());
		}
	}

	void FemasOrderSession::unsubscribe(const Subscription& subscription, const std::shared_ptr<IConsumer<OrderSummaryMessage> >& consumer)
	{
		PublisherAdapter<MarketDataMessage>::unsubscribe(subscription, consumer);

		fmt::format_int format(subscription.getKey());

		// femas requires char* without writing to it
		char* request[1] = {const_cast<char*> (format.c_str())};
		int result = session->UnSubMarketData(request, 1);

		if (result != 0)
		{
			LOGERR("Error unsubscribing from {}", subscription.getKey());
		}
	}

	void FemasOrderSession::sendOrder(const std::shared_ptr<OrderMessage>& message)
	{

	}

	void FemasOrderSession::cancel(const std::shared_ptr<OrderMessage>& message)
	{

	}

	void FemasOrderSession::OnFrontConnected()
	{
		LOGDEBUG("Market data session connected.");
	}

	void FemasOrderSession::OnFrontDisconnected(int reason)
	{
		LOGDEBUG("Market data session disconnected on {}, auto-reconnecting", reason);
	}

	void FemasOrderSession::OnHeartBeatWarning(int timeLapse)
	{
		LOGWARN("No heart beat in {} seconds.", timeLapse);
	}

	void FemasOrderSession::OnPackageStart(int topicID, int sequenceNo)
	{
		LOGTRACE("Received packet {} on topic {}", sequenceNo, topicID);
	}

	void FemasOrderSession::OnPackageEnd(int topicID, int sequenceNo)
	{
		LOGTRACE("Finished packet {} on topic {}", sequenceNo, topicID);
	}

	void FemasOrderSession::OnRspError(CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		LOGERR("Error on request {} : {}, {}", requestID, info->ErrorID, info->ErrorMsg);
	}

	void FemasOrderSession::OnRspUserLogin(CUstpFtdcRspUserLoginField *userLogin, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("User {} logged in successfully", userLogin->UserID);
		}
		else
		{
			LOGERR("Login attempt failed. User: {}, error: {}, {}", userLogin->UserID, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasOrderSession::OnRspUserLogout(CUstpFtdcRspUserLogoutField *userLogout, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("User {} logged out successfully", userLogin->UserID);
		}
		else
		{
			LOGERR("Logout attempt failed. User: {}, error: {}, {}", userLogin->UserID, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasOrderSession::OnRspSubscribeTopic(CUstpFtdcDisseminationField *dissemination, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
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

	void FemasOrderSession::OnRspQryTopic(CUstpFtdcDisseminationField *dissemination, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
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

	void OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *userPasswordUpdate, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("User {} password updated from {} to {}", userPasswordUpdate->UserID, userPasswordUpdate->OldPassword, userPasswordUpdate->NewPassword);
		}
		else
		{
			LOGERR("User {} password update failed", userPasswordUpdate->UserID);
		}
	}

	void OnRspOrderInsert(CUstpFtdcInputOrderField *inputOrder, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspOrderAction(CUstpFtdcOrderActionField *orderAction, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *flowMessageCancel)
	{

	}

	void OnRtnTrade(CUstpFtdcTradeField *trade)
	{

	}

	void OnRtnOrder(CUstpFtdcOrderField *order)
	{

	}

	void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *inputOrder, CUstpFtdcRspInfoField *rspInfo)
	{

	}

	void OnErrRtnOrderAction(CUstpFtdcOrderActionField *orderAction, CUstpFtdcRspInfoField *rspInfo)
	{

	}

	void OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *instrumentStatus)
	{

	}

	void OnRtnInvestorAccountDeposit(CUstpFtdcInvestorAccountDepositResField *investorAccountDepositRes)
	{

	}

	void OnRspQryOrder(CUstpFtdcOrderField *order, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryTrade(CUstpFtdcTradeField *trade, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *userInvestor, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryTradingCode(CUstpFtdcRspTradingCodeField *tradingCode, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *investorAccount, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryInstrument(CUstpFtdcRspInstrumentField *instrument, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryExchange(CUstpFtdcRspExchangeField *exchange, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *investorPosition, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryComplianceParam(CUstpFtdcRspComplianceParamField *complianceParam, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *investorFee, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

	void OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *investorMargin, CUstpFtdcRspInfoField *rspInfo, int requestID, bool isLast)
	{

	}

}



