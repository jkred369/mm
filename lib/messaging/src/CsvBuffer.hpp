/*
 * CsvBuffer.hpp
 *
 *  Created on: Sep 28, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_CSVBUFFER_HPP_
#define LIB_MESSAGING_SRC_CSVBUFFER_HPP_

#include <sstream>

#include <StringUtil.hpp>

namespace mm
{
	//
	// This class implements a buffer concept supporting input and output.
	//
	class CsvBuffer
	{

	private:

		std::stringstream ss;
	};
}


#endif /* LIB_MESSAGING_SRC_CSVBUFFER_HPP_ */
