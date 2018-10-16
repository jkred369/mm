/*
 * OrderIdGenerator.hpp
 *
 *  Created on: Oct 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_ARB_SRC_ALGOUTIL_HPP_
#define LIB_ARB_SRC_ALGOUTIL_HPP_

#include "OrderIdGenerator.hpp"

namespace mm
{
	//
	// This class defines all the algo utility functions.
	//
	struct AlgoUtil
	{
		//
		// Get the default order ID generator.
		//
		OrderIdGenerator& getDefaultOrderIdGenerator()
		{
			static OrderIdGenerator generator;
			return generator;
		}
	};

}



#endif /* LIB_ARB_SRC_ALGOUTIL_HPP_ */
