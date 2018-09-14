/*
 * FemasMarketDataSession.cpp
 *
 *  Created on: Sep 13, 2018
 *      Author: suoalex
 */

#include <Log.hpp>

#include <FemasMarketDataSession.hpp>

namespace mm
{
	FemasMarketDataSession::FemasMarketDataSession() :
		session(CUstpFtdcMduserApi::CreateFtdcMduserApi())
	{
	}

	FemasMarketDataSession::~FemasMarketDataSession()
	{
	}

	void FemasMarketDataSession::OnFrontConnected()
	{
		LOGDEBUG << "Market data session connected." << ENDLOG;
	}

	void FemasMarketDataSession::OnFrontDisconnected(int reason)
	{
		LOGDEBUG << "Market data session disconnected on " << reason << ", auto-reconnecting" << ENDLOG;
	}

	void FemasMarketDataSession::OnHeartBeatWarning(int timeLapse)
	{
		LOGWARN << "no heart beat in " << timeLapse << " seconds." << ENDLOG;
	}

	void OnPackageStart(int topicID, int sequenceNo)
	{
		LOGTRACE << "Received packet " << sequenceNo << " on topic " << topicID << ENDLOG;
	}

	void FemasMarketDataSession::OnPackageEnd(int topicID, int sequenceNo)
	{
		LOGTRACE << "Finished packet " << sequenceNo << " on topic " << topicID << ENDLOG;
	}

	void FemasMarketDataSession::OnRspError(CUstpFtdcRspInfoField *info, int requestID, bool isLast)
	{
	}

	void FemasMarketDataSession::OnRspUserLogin(
			CUstpFtdcRspUserLoginField *userLogin,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
	{
	}

	void FemasMarketDataSession::OnRspUserLogout(
			CUstpFtdcRspUserLogoutField *userLogout,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
	{
	}

	void FemasMarketDataSession::OnRspSubscribeTopic(
			CUstpFtdcDisseminationField *dissemination,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
	{
	}

	void FemasMarketDataSession::OnRspQryTopic(
			CUstpFtdcDisseminationField *dissemination,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
	{
	}

	void FemasMarketDataSession::OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *depthMarketData)
	{
		MarketDataMessage message;

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
		message.levels[Side::BID][0].price = depthMarketData->BidPrice1;
		message.levels[Side::BID][0].qty = depthMarketData->BidVolume1;

		message.levels[Side::BID][1].price = depthMarketData->BidPrice2;
		message.levels[Side::BID][1].qty = depthMarketData->BidVolume2;

		message.levels[Side::BID][2].price = depthMarketData->BidPrice3;
		message.levels[Side::BID][2].qty = depthMarketData->BidVolume3;

		message.levels[Side::BID][3].price = depthMarketData->BidPrice4;
		message.levels[Side::BID][3].qty = depthMarketData->BidVolume4;

		message.levels[Side::BID][4].price = depthMarketData->BidPrice5;
		message.levels[Side::BID][4].qty = depthMarketData->BidVolume5;

		message.levels[Side::ASK][0].price = depthMarketData->AskPrice1;
		message.levels[Side::ASK][0].qty = depthMarketData->AskVolume1;

		message.levels[Side::ASK][1].price = depthMarketData->AskPrice2;
		message.levels[Side::ASK][1].qty = depthMarketData->AskVolume2;

		message.levels[Side::ASK][2].price = depthMarketData->AskPrice3;
		message.levels[Side::ASK][2].qty = depthMarketData->AskVolume3;

		message.levels[Side::ASK][3].price = depthMarketData->AskPrice4;
		message.levels[Side::ASK][3].qty = depthMarketData->AskVolume4;

		message.levels[Side::ASK][4].price = depthMarketData->AskPrice5;
		message.levels[Side::ASK][4].qty = depthMarketData->AskVolume5;
	}

	void FemasMarketDataSession::OnRspSubMarketData(
			CUstpFtdcSpecificInstrumentField *specificInstrument,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
	{
	}

	void FemasMarketDataSession::OnRspUnSubMarketData(
			CUstpFtdcSpecificInstrumentField *specificInstrument,
			CUstpFtdcRspInfoField *info,
			int requestID,
			bool isLast)
	{
	}

}
