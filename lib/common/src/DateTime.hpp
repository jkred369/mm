/*
 * DateTime.hpp
 *
 *  Created on: Oct 20, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_DATETIME_HPP_
#define LIB_COMMON_SRC_DATETIME_HPP_

#include <boost/date_time/posix_time/posix_time.hpp>

namespace mm
{
	typedef boost::gregorian::date Date;
	typedef boost::posix_time::ptime DateTime;

	//
	// The struct defines the common utility functions for date.
	//
	struct DateUtil
	{
		// The epoch date.
		static const Date epochDate;

		//
		// Get today's date.
		//
		// return : today's date.
		//
		static inline Date today()
		{
			return boost::gregorian::day_clock::local_day();
		}

		//
		// convert an int in yyyymmdd format to a date.
		//
		// yyyymmdd : The integer.
		//
		// return : The date object from the integer.
		//
		static inline Date intToDate(const int yyyymmdd)
		{
			// compiler will optimize this no worries
			const int year = yyyymmdd / 10000;
			const int month = (yyyymmdd - year * 10000) / 100;
			const int day = yyyymmdd % 100;

			return Date(year, month, day);
		}

		//
		// convert a date to yyyymmdd format as integer.
		//
		// date : The date.
		//
		// return : The integer as yyyymmdd format.
		//
		static inline int dateToInt(const Date& date)
		{
			const int year = date.year();
			const int month = date.month();
			const int day = date.day();

			return year * 10000 + month * 100 + day;
		}
	};

	//
	// The struct defines the common utility functions for datetime.
	//
	struct DateTimeUtil
	{
		// The epoch daet time.
		static const DateTime epochDateTime;

		//
		// Get the current local date time.
		//
		// return : current local date time.
		//
		static inline DateTime now()
		{
			return boost::posix_time::microsec_clock::local_time();
		}

		//
		// Get the current UTC date time.
		//
		// return : current UTC date time.
		//
		static inline DateTime utc()
		{
			return boost::posix_time::microsec_clock::universal_time();
		}
	};
}



#endif /* LIB_COMMON_SRC_DATETIME_HPP_ */
