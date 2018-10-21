/*
 * FemasOrderSession.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: suoalex
 */

#include <EnumType.hpp>
#include <NativeDefinition.hpp>
#include <ProductService.hpp>
#include <StringUtil.hpp>

#include "FemasOrderSession.hpp"

mm::Logger mm::FemasOrderSession::logger;

namespace mm
{
	FemasOrderSession::FemasOrderSession(
			const KeyType dispatchKey,
			const std::string serviceName,
			ServiceContext& serviceContext,
			const std::string productServiceName,
			const FemasUserDetail& userDetail,
			const FemasOrderDetail& orderDetail,
			const int cpuAffinity) :
		OrderManager<FemasOrderSession, NullObjectPool>(dispatchKey, serviceName, serviceContext, *this),
		userDetail(userDetail),
		orderDetail(orderDetail),
		cpuAffinity(cpuAffinity),
		dispatcher(serviceContext.getDispatcher()),
		executionReportPool(POOL_SIZE),
		session(CUstpFtdcTraderApi::CreateFtdcTraderApi()),
		stopFlag(false),
		requestId(0)
	{
		static_assert(BID >= 0, "Bid index must be positive.");
		static_assert(ASK >= 0, "Ask index must be positive.");

		// TODO : Make the product update dynamic.
		{
			ProductService* productService;
			if (!serviceContext.getService(productServiceName, productService))
			{
				LOGERR("Order session failed to get product service from service context");
				throw std::runtime_error("Failed to create Femas order session. Cannot find product service with name: " + productServiceName);
			}

			productService->initSnapshot(this);
			LOGINFO("Symbol map created with {} products.", symbolMap.size());
		}

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

	FemasOrderSession::~FemasOrderSession()
	{
		stop();
	}

	bool FemasOrderSession::start()
	{
		if (!OrderManager<FemasOrderSession, NullObjectPool>::start())
		{
			return false;
		}

		// wait on init latch to make sure the connection is established
		LOGINFO("Waiting for connection...")
		initLatch.wait();

		// login attempt
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

		if (result == 0)
		{
			LOGINFO("Order session logged in as {}", userDetail.userId);
			return true;
		}
		else
		{
			LOGERR("Error loging as user {} with code: {}", userDetail.userId, result);
			return false;
		}

		// query the necessary information
		{
			CUstpFtdcQryUserInvestorField field;
			StringUtil::copy(field.BrokerID, userDetail.brokerId, sizeof(field.BrokerID));
			StringUtil::copy(field.UserID, userDetail.userId, sizeof(field.UserID));

			session->ReqQryUserInvestor(&field, ++requestId);
		}

		{
			CUstpFtdcQryInvestorPositionField field;
			StringUtil::copy(field.BrokerID, userDetail.brokerId, sizeof(field.BrokerID));
			StringUtil::copy(field.UserID, userDetail.userId, sizeof(field.UserID));

			session->ReqQryInvestorPosition(&field, ++requestId);
		}

		LOGINFO("Waiting for data querying...");
		startLatch.wait();

		// TODO: determine if resume is proper here.
		// according to doc, user topic == private topic.
		session->SubscribePublicTopic(USTP_TERT_RESUME);
		session->SubscribeUserTopic(USTP_TERT_RESUME);
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

		// delegate
		OrderManager<FemasOrderSession, NullObjectPool>::stop();
	}

	void FemasOrderSession::consume(const std::shared_ptr<const Product>& message)
	{
		const ProductMessage& content = message->getContent();

		if (content.symbol.stringSize() > sizeof(TUstpFtdcInstrumentIDType))
		{
			LOGERR("Error adding product {}: symbol too long: {}", content.id, content.symbol.toString());
			return;
		}

		std::memset(symbolMap[content.id], 0, sizeof(TUstpFtdcInstrumentIDType));
		std::strncpy(symbolMap[content.id], content.symbol.data(), sizeof(TUstpFtdcInstrumentIDType));
	}

	void FemasOrderSession::sendOrder(const std::shared_ptr<const OrderMessage>& message)
	{
		// fixed values to prevent if blocks
		constexpr TUstpFtdcDirectionType direction[] = {USTP_FTDC_D_Buy, USTP_FTDC_D_Sell};
		constexpr TUstpFtdcTimeConditionType timeCondition[] = {USTP_FTDC_TC_GFD, USTP_FTDC_TC_GFD, USTP_FTDC_TC_IOC, USTP_FTDC_TC_GFD};
		constexpr TUstpFtdcVolumeConditionType volumeCondition[] = {USTP_FTDC_VC_AV, USTP_FTDC_VC_AV, USTP_FTDC_VC_AV, USTP_FTDC_VC_CV};
		constexpr TUstpFtdcOffsetFlagType offsetType[] = {USTP_FTDC_OF_Open, USTP_FTDC_OF_Close, USTP_FTDC_OF_ForceClose, USTP_FTDC_OF_CloseToday, USTP_FTDC_OF_CloseYesterday};

		CUstpFtdcInputOrderField order;
		std::memset(&order, 0, sizeof(order));

		// hard code values
		order.OrderPriceType = USTP_FTDC_OPT_LimitPrice;
		order.OrderSysID[0] = '\0';
		order.UserCustom[0] = '\0';

		// session specific
		order.HedgeFlag = orderDetail.hedgeFlag;
		order.IsAutoSuspend = orderDetail.isAutoSuspend;

		StringUtil::copy(order.BrokerID, userDetail.brokerId, sizeof(order.BrokerID));
		StringUtil::copy(order.ExchangeID, exchangeId, sizeof(order.ExchangeID));
		StringUtil::copy(order.GTDDate, tradingDate, sizeof(order.GTDDate));
		StringUtil::copy(order.InvestorID, investorId, sizeof(order.InvestorID));
		StringUtil::copy(order.UserID, userDetail.userId, sizeof(order.UserID));

		// order specific
		order.LimitPrice = message->price;
		order.MinVolume = 0;
		order.StopPrice = message->price;
		order.Volume = message->totalQty;

		order.Direction = direction[toValue(message->side)];
		order.OffsetFlag = offsetType[toValue(message->offsetType)];
		order.TimeCondition = timeCondition[toValue(message->type)];
		order.VolumeCondition = volumeCondition[toValue(message->type)];

		StringUtil::fromInt(order.UserOrderLocalID, message->orderId, sizeof(order.UserOrderLocalID));

		// ID mapping
		{
			auto it = symbolMap.find(message->instrumentId);
			if (UNLIKELY(it == symbolMap.end()))
			{
				LOGERR("Cannot find symbol mapping for order {} on instrument {}.", message->orderId, message->instrumentId);
				return;
			}

			std::memcpy(order.InstrumentID, it->second, sizeof(order.InstrumentID));
		}

		// request
		const int result = session->ReqOrderInsert(&order, ++requestId);
		if (UNLIKELY(result != 0))
		{
			LOGERR("Error sending order {} on instrument {}: {}", message->orderId, message->instrumentId, result);
		}
	}

	void FemasOrderSession::cancel(const std::shared_ptr<const OrderMessage>& message)
	{
		CUstpFtdcOrderActionField action;
		std::memset(&action, 0, sizeof(action));

		// hardcoded
		action.ActionFlag = USTP_FTDC_AF_Delete;
		action.LimitPrice = 0.0;
		action.UserOrderActionLocalID[0] = '0';
		action.VolumeChange = 0;

		// sesson specific
		StringUtil::copy(action.BrokerID, userDetail.brokerId, sizeof(action.BrokerID));
		StringUtil::copy(action.ExchangeID, exchangeId, sizeof(action.ExchangeID));
		StringUtil::copy(action.InvestorID, investorId, sizeof(action.InvestorID));
		StringUtil::copy(action.UserID, userDetail.userId, sizeof(action.UserID));

		// order specific
		StringUtil::fromInt(action.UserOrderLocalID, message->orderId, sizeof(action.UserOrderLocalID));

		if (UNLIKELY(!getExchangeOrderId(message->orderId, action.OrderSysID)))
		{
			LOGERR("Cannot send cancel with exchange ID look up failure for order: {}", message->orderId);
			return;
		}

		// request
		const int result = session->ReqOrderAction(&action, ++requestId);
		if (UNLIKELY(result != 0))
		{
			LOGERR("Error sending cancel on order {}, instrument {}: {}", message->orderId, message->instrumentId, result);
		}
	}

	void FemasOrderSession::OnFrontConnected()
	{
		// here is the first chance we could set affinity for the callback thread
		if (cpuAffinity != ThreadUtil::CPU_ID_NOT_SET)
		{
			ThreadUtil::setAffinity(cpuAffinity);
			LOGINFO("Order callback thread pin to core {}", cpuAffinity);
		}

		initLatch.countDown();
		LOGDEBUG("Order session connected.");
	}

	void FemasOrderSession::OnFrontDisconnected(int reason)
	{
		LOGDEBUG("Order session disconnected on {}, auto-reconnecting", reason);
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
			LOGINFO("User {} logged out successfully", userLogout->UserID);
		}
		else
		{
			LOGERR("Logout attempt failed. User: {}, error: {}, {}", userLogout->UserID, info->ErrorID, info->ErrorMsg);
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
			const std::string femasInvestorId = userInvestor->InvestorID;

			dispatcher.submit(dispatchKey, [this, femasInvestorId] () {
				investorId = femasInvestorId;
				LOGINFO("Investor ID set to: {}", investorId);
			});
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



