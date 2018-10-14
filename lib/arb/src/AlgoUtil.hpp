/*
 * OrderIdGenerator.hpp
 *
 *  Created on: Oct 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_ARB_SRC_ALGOUTIL_HPP_
#define LIB_ARB_SRC_ALGOUTIL_HPP_

#include <ctime>

#include <IdGenerator.hpp>
#include <NullMutex.hpp>
#include <TimeUtil.hpp>

namespace mm
{
	//
	// This class defines all the algo utility functions.
	//
	struct AlgoUtil
	{
		//
		// Create an order ID generator.
		//
		// strategyId : The strategy ID using this generator.
		//
		static NumericalIdGenerator<NullMutex> createOrderIdGenerator(std::int64_t strategyId)
		{
			if (strategyId > 100)
			{
				throw std::invalid_argument("Cannot generate for strategy ID > 100.");
			}

			const std::int64_t prefix = getTimeAsInt(std::time(0)) * 100 + strategyId;
			return NumericalIdGenerator<NullMutex>(prefix, 1000000);
		}
	};

}



#endif /* LIB_ARB_SRC_ALGOUTIL_HPP_ */
