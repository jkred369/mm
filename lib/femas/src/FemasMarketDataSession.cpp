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
