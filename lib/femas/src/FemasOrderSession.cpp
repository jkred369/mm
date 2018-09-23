/*
 * FemasOrderSession.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: suoalex
 */

#include <fmt/format.h>

#include <EnumType.hpp>
#include <NativeDefinition.hpp>
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
		// fixed values to prevent if blocks
		constexpr char direction[] = {USTP_FTDC_D_Buy, USTP_FTDC_D_Sell};
		constexpr char timeCondition[] = {USTP_FTDC_TC_GFD, USTP_FTDC_TC_GFD, USTP_FTDC_TC_IOC, USTP_FTDC_TC_GFD};
		constexpr char volumeCondition[] = {USTP_FTDC_VC_AV, USTP_FTDC_VC_AV, USTP_FTDC_VC_AV, USTP_FTDC_VC_CV};

		CUstpFtdcInputOrderField order;
		std::memset(&order, 0, sizeof(order));

		// TODO: clarify if we need BusinessUnit
		// TODO: clarify on off set flag
		// TODO: determine hedge flag.

		// hard code values
		order.HedgeFlag = USTP_FTDC_CHF_Arbitrage;
		order.IsAutoSuspend = 0;
		order.OffsetFlag = USTP_FTDC_OF_Open;
		order.OrderPriceType = USTP_FTDC_OPT_LimitPrice;
		order.OrderSysID = '\0';
		order.UserCustom[0] = '\0';

		// session specific
		StringUtil::copy(order.BrokerID, userDetail.ipAddress, sizeof(order.BrokerID));
		StringUtil::copy(order.ExchangeID, exchangeId, sizeof(order.ExchangeID));
		StringUtil::copy(order.GTDDate, tradingDate, sizeof(order.GTDDate));
		StringUtil::copy(order.InvestorID, investorId, sizeof(order.InvestorID));
		StringUtil::copy(order.UserID, userDetail.userId, sizeof(order.UserID));

		// order specific
		order.LimitPrice = message->price;
		order.MinVolume = 0;
		order.StopPrice = message->price;
		order.Volume = message->qty;

		order.Direction = direction[toValue(message->side)];
		order.TimeCondition = timeCondition[toValue(message->type)];
		order.VolumeCondition = volumeCondition[toValue(message->type)];

		StringUtil::fromInt(order.InstrumentID, message->instrumentId, sizeof(order.InstrumentID));
		StringUtil::fromInt(order.UserOrderLocalID, message->orderId, sizeof(order.UserOrderLocalID));

		const int result = session->ReqOrderInsert(&order, ++requestId);
		if (UNLIKELY(result != 0))
		{
			LOGERR("Error sending order {} on instrument {}: {}", message->orderId, message->instrumentId, result);
		}
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
			tradingDate = userLogin->TradingDay;
			LOGINFO("User {} logged in successfully on date {}", userLogin->UserID, tradingDate);
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

	void FemasOrderSession::OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *userPasswordUpdate, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
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

	void FemasOrderSession::OnRspOrderInsert(CUstpFtdcInputOrderField *inputOrder, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRspOrderAction(CUstpFtdcOrderActionField *orderAction, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *flowMessageCancel)
	{

	}

	void FemasOrderSession::OnRtnTrade(CUstpFtdcTradeField *trade)
	{

	}

	void FemasOrderSession::OnRtnOrder(CUstpFtdcOrderField *order)
	{

	}

	void FemasOrderSession::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *inputOrder, CUstpFtdcRspInfoField *info)
	{

	}

	void FemasOrderSession::OnErrRtnOrderAction(CUstpFtdcOrderActionField *orderAction, CUstpFtdcRspInfoField *info)
	{

	}

	void FemasOrderSession::OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *instrumentStatus)
	{

	}

	void FemasOrderSession::OnRtnInvestorAccountDeposit(CUstpFtdcInvestorAccountDepositResField *investorAccountDepositRes)
	{

	}

	void FemasOrderSession::OnRspQryOrder(CUstpFtdcOrderField *order, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRspQryTrade(CUstpFtdcTradeField *trade, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *userInvestor, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			investorId = userInvestor->InvestorID;
			LOGINFO("Investor ID set to {}", investorId);
		}
		else
		{
			LOGERR("Error querying investor ID for user {}: {}, {}", userInvestor->UserID, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasOrderSession::OnRspQryTradingCode(CUstpFtdcRspTradingCodeField *tradingCode, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
	}

	void FemasOrderSession::OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *investorAccount, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRspQryInstrument(CUstpFtdcRspInstrumentField *instrument, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRspQryExchange(CUstpFtdcRspExchangeField *exchange, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		if (info->ErrorID == 0)
		{
			exchangeId = exchange->ExchangeID;
		}
		else
		{
			LOGERR("Error getting exchange ID with error: {}, {}", info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasOrderSession::OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *investorPosition, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRspQryComplianceParam(CUstpFtdcRspComplianceParamField *complianceParam, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *investorFee, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

	void FemasOrderSession::OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *investorMargin, CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{

	}

}



