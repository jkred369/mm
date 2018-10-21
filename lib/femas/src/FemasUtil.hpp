/*
 * FemasUtil.hpp
 *
 *  Created on: Oct 20, 2018
 *      Author: suoalex
 */

#ifndef LIB_FEMAS_SRC_FEMASUTIL_HPP_
#define LIB_FEMAS_SRC_FEMASUTIL_HPP_

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>

#include <Currency.hpp>
#include <DateTime.hpp>
#include <Exchange.hpp>
#include <ProductType.hpp>

#include <femas/USTPFtdcUserApiDataType.h>
#include <femas/USTPFtdcUserApiStruct.h>

namespace mm
{
	//
	// This class contains all the utility functions about femas specifically.
	//
	struct FemasUtil
	{
		//
		// Determine currency from given data.
		//
		// field : The data fields.
		//
		// return : The currency type.
		//
		static inline Currency getCurrency(const TUstpFtdcCurrencyType& currency)
		{
			if (currency == USTP_FTDC_C_RMB)
			{
				return Currency::CNY;
			}
			else if (currency == USTP_FTDC_C_UDOLLAR)
			{
				return Currency::USD;
			}

			throw std::invalid_argument("Cannot determine currency for value: " + std::to_string((int)currency));
		}

		//
		// Determine exchange from given data.
		//
		// field : The data fields.
		//
		// return : The exchange value.
		//
		static inline Exchange getExchange(const TUstpFtdcExchangeIDType& exchangeId)
		{
			static const std::array<std::pair<const char*, Exchange>, 13 > pairs = {{
					{"SSE", Exchange::SSE}, {"SZSE", Exchange::SZSE}, {"CFFEX", Exchange::CFFEX}, {"SHFE", Exchange::SHFE},
					{"CZCE", Exchange::CZCE}, {"DCE", Exchange::DCE}, {"INE", Exchange::INE}, {"SGE", Exchange::SGE},
					{"HKEX", Exchange::HKEX}, {"SMART", Exchange::SMART}, {"GLOBEX", Exchange::GLOBEX}, {"IDEALPRO", Exchange::IDEALPRO},
					{"OANDA", Exchange::OANDA}
			}};

			const char* id = exchangeId;
			auto it = std::find_if(pairs.begin(), pairs.end(), [id] (const std::pair<const char*, Exchange>& pair) {
				return std::strcmp(pair.first, id) == 0;
			});

			if (it != pairs.end())
			{
				return it->second;
			}

			throw std::invalid_argument("Cannot interpret exchange name: " + std::string(exchangeId));
		}

		//
		// Get an internal date representation from the femas date.
		//
		// femasDate : reference to femas date structure.
		//
		// return : The date in internal representation.
		//
		static inline Date getDate(const TUstpFtdcDateType& femasDate)
		{
			return DateUtil::intToDate(std::atoi(femasDate));
		}
	};

}



#endif /* LIB_FEMAS_SRC_FEMASUTIL_HPP_ */
