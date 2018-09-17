/*
 * FemasMarketDataSession.cpp
 *
 *  Created on: Sep 13, 2018
 *      Author: suoalex
 */

#include <fmt/format.h>

#include "FemasMarketDataSession.hpp"

mm::Logger mm::FemasMarketDataSession::logger;

namespace mm
{
	FemasMarketDataSession::FemasMarketDataSession() :
		session(CUstpFtdcMduserApi::CreateFtdcMduserApi()),
		stopFlag(false)
	{
		static_assert(BID >= 0, "Bid index must be positive.");
		static_assert(ASK >= 0, "Ask index must be positive.");

		// TODO determine if need to pass a config file to create API
		session->RegisterSpi(this);
		session->Init();
	}

	FemasMarketDataSession::~FemasMarketDataSession()
	{
		stop();
	}

	bool FemasMarketDataSession::start()
	{
		// TODO implement this
		return true;
	}

	void FemasMarketDataSession::stop()
	{
		// sanity check
		{
			bool expected = false;
			if (!stopFlag.compare_exchange_strong(expected, true))
			{
				return;
			}
		}

		// actually stop the service
		// it's commented that session->join() isn't stable?
		session->RegisterSpi(nullptr);
		session->Release();
		session = nullptr;
	}

	void FemasMarketDataSession::subscribe(const ISubscription<std::int64_t>& subscription)
	{
		fmt::format_int format(subscription.getKey());

		// femas requires char* without writing to it
		char* request[1] = {const_cast<char*> (format.c_str())};
		const int result = session->SubMarketData(request, 1);

		if (result != 0)
		{
			LOGERR("Error subscribing to {}", subscription.getKey());
		}
	}

	void FemasMarketDataSession::unsubscribe(const ISubscription<std::int64_t>& subscription)
	{
		fmt::format_int format(subscription.getKey());

		// femas requires char* without writing to it
		char* request[1] = {const_cast<char*> (format.c_str())};
		int result = session->UnSubMarketData(request, 1);

		if (result != 0)
		{
			LOGERR("Error unsubscribing from {}", subscription.getKey());
		}
	}

	void FemasMarketDataSession::OnFrontConnected()
	{
		LOGDEBUG("Market data session connected.");
	}

	void FemasMarketDataSession::OnFrontDisconnected(int reason)
	{
		LOGDEBUG("Market data session disconnected on {}, auto-reconnecting", reason);
	}

	void FemasMarketDataSession::OnHeartBeatWarning(int timeLapse)
	{
		LOGWARN("No heart beat in {} seconds.", timeLapse);
	}

	void FemasMarketDataSession::OnPackageStart(int topicID, int sequenceNo)
	{
		LOGTRACE("Received packet {} on topic {}", sequenceNo, topicID);
	}

	void FemasMarketDataSession::OnPackageEnd(int topicID, int sequenceNo)
	{
		LOGTRACE("Finished packet {} on topic {}", sequenceNo, topicID);
	}

	void FemasMarketDataSession::OnRspError(CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
		LOGERR("Error on request {} : {}, {}", requestID, info->ErrorID, info->ErrorMsg);
	}

	void FemasMarketDataSession::OnRspUserLogin(
			CUstpFtdcRspUserLoginField *userLogin,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
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

	void FemasMarketDataSession::OnRspUserLogout(
			CUstpFtdcRspUserLogoutField *userLogout,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
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

	void FemasMarketDataSession::OnRspSubscribeTopic(
			CUstpFtdcDisseminationField *dissemination,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
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

	void FemasMarketDataSession::OnRspQryTopic(
			CUstpFtdcDisseminationField *dissemination,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
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

	void FemasMarketDataSession::OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *depthMarketData)
	{
		std::shared_ptr<MarketDataMessage> messagePointer = getMessage();
		MarketDataMessage& message = *messagePointer;

		// level 1 fields
		message.instrumentId = depthMarketData->InstrumentID;

		message.open = depthMarketData->OpenPrice;
		message.close = depthMarketData->ClosePrice;
		message.high = depthMarketData->HighestPrice;
		message.low = depthMarketData->LowestPrice;
		message.last = depthMarketData->LastPrice;
		message.highLimit = depthMarketData->UpperLimitPrice;
		message.lowLimit = depthMarketData->LowerLimitPrice;
		message.volume = depthMarketData->Volume;
		message.turnover = depthMarketData->Turnover;

		// market data levels - silly yes but fast
		message.levels[BID][0].price = depthMarketData->BidPrice1;
		message.levels[BID][0].qty = depthMarketData->BidVolume1;

		message.levels[BID][1].price = depthMarketData->BidPrice2;
		message.levels[BID][1].qty = depthMarketData->BidVolume2;

		message.levels[BID][2].price = depthMarketData->BidPrice3;
		message.levels[BID][2].qty = depthMarketData->BidVolume3;

		message.levels[BID][3].price = depthMarketData->BidPrice4;
		message.levels[BID][3].qty = depthMarketData->BidVolume4;

		message.levels[BID][4].price = depthMarketData->BidPrice5;
		message.levels[BID][4].qty = depthMarketData->BidVolume5;

		message.levels[ASK][0].price = depthMarketData->AskPrice1;
		message.levels[ASK][0].qty = depthMarketData->AskVolume1;

		message.levels[ASK][1].price = depthMarketData->AskPrice2;
		message.levels[ASK][1].qty = depthMarketData->AskVolume2;

		message.levels[ASK][2].price = depthMarketData->AskPrice3;
		message.levels[ASK][2].qty = depthMarketData->AskVolume3;

		message.levels[ASK][3].price = depthMarketData->AskPrice4;
		message.levels[ASK][3].qty = depthMarketData->AskVolume4;

		message.levels[ASK][4].price = depthMarketData->AskPrice5;
		message.levels[ASK][4].qty = depthMarketData->AskVolume5;

		notify(messagePointer);
	}

	void FemasMarketDataSession::OnRspSubMarketData(
			CUstpFtdcSpecificInstrumentField *specificInstrument,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("Subscription established for {}", specificInstrument->InstrumentID);
		}
		else
		{
			LOGERR("Error establishing subscription on {}: {}, {}", specificInstrument->InstrumentID, info->ErrorID, info->ErrorMsg);
		}
	}

	void FemasMarketDataSession::OnRspUnSubMarketData(
			CUstpFtdcSpecificInstrumentField *specificInstrument,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
	{
		if (info->ErrorID == 0)
		{
			LOGINFO("Unsubscription finished for {}", specificInstrument->InstrumentID);
		}
		else
		{
			LOGERR("Error unsubscribing on {}: {}, {}", specificInstrument->InstrumentID, info->ErrorID, info->ErrorMsg);
		}
	}

}
