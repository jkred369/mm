/*
 * FemasMarketDataSession.hpp
 *
 *  Created on: Sep 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_FEMAS_SRC_FEMASMARKETDATASESSION_HPP_
#define LIB_FEMAS_SRC_FEMASMARKETDATASESSION_HPP_

#include <atomic>
#include <memory>
#include <string>

#include <femas/USTPFtdcMduserApi.h>

#include <EnumType.hpp>
#include <IService.hpp>
#include <Logger.hpp>
#include <MarketDataMessage.hpp>
#include <PublisherAdapter.hpp>

#include <FemasUserDetail.hpp>

namespace mm
{
	//
	// This class defines a Femas market data session for market data updates.
	//
	class FemasMarketDataSession :
			public IService,
			public CUstpFtdcMduserSpi,
			public PublisherAdapter<MarketDataMessage>
	{
	public:

		//
		// Constructor.
		//
		// userDetail : The configuration details.
		//
		FemasMarketDataSession(const FemasUserDetail& detail);

		//
		// Destructor.
		//
		virtual ~FemasMarketDataSession();

		//
		// service interface.
		//
		virtual bool start() override;
		virtual void stop() override;

		//
		// publisher functionality.
		//
		virtual void subscribe(const Subscription& subscription, const std::shared_ptr<IConsumer<MarketDataMessage> >& consumer) override;
		virtual void unsubscribe(const Subscription& subscription, const std::shared_ptr<IConsumer<MarketDataMessage> >& consumer) override;

		//
		// Fired when the session is connected.
		//
		virtual void OnFrontConnected() override;

		//
		// Fired when the session is disconnected.
		//
		// reason : The reason number.
		//
		virtual void OnFrontDisconnected(int reason) override;

		//
		// Fired when the heart beat got error.
		//
		// timeLapse : The time passed.
		//
		virtual void OnHeartBeatWarning(int timeLapse) override;

		//
		// Fired when the packet start is received.
		//
		// topicID : The topic on which the packet is received.
		// sequenceNo : The sequence number.
		//
		virtual void OnPackageStart(int topicID, int sequenceNo) override;

		//
		// Fired when the packet end is received.
		//
		// topicID : The topic on which the packet is received.
		// sequenceNo : The sequence number.
		//
		virtual void OnPackageEnd(int topicID, int sequenceNo) override;

		//
		// Fired when the response got error.
		//
		// info : Structure with details.
		// requestID : The request got the error.
		// isLast : Flag if this is the last request.
		//
		virtual void OnRspError(CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the login response.
		//
		// userLogin : details for the login
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *userLogin, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the logout response.
		//
		// userLogout : details for the logout
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *userLogout, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the subscription response.
		//
		// dissemination : details for the subscription.
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspSubscribeTopic(CUstpFtdcDisseminationField *dissemination, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the topic query response.
		//
		// dissemination : details for the subscription.
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryTopic(CUstpFtdcDisseminationField *dissemination, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the market data depth update.
		//
		// depthMarketData : market data depth update.
		//
		virtual void OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *depthMarketData) override;

		//
		// Fired for the market data subscription response.
		//
		// instrument : instrument subscribed.
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *instrument, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the market data unsubscription response.
		//
		// instrument : instrument unsubscribed.
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *instrument, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

	private:

		inline std::shared_ptr<MarketDataMessage> getMessage()
		{
			return std::make_shared<MarketDataMessage>(new MarketDataMessage());
		}

		// The int value for bid
		static constexpr int BID = toValue(Side::BID);

		// The int value for ask
		static constexpr int ASK = toValue(Side::ASK);

		// The logger for this class.
		static Logger logger;

		// The login user detail.
		const FemasUserDetail userDetail;

		// The actual API session.
		// Note that we cannot use unique_ptr etc here as the destructor is protected.
		CUstpFtdcMduserApi* session;

		// Flag if stop is called or in process.
		std::atomic<bool> stopFlag;

		// The request ID value keeper.
		std::atomic<int> requestId;
	};
}



#endif /* LIB_FEMAS_SRC_FEMASMARKETDATASESSION_HPP_ */
