/*
 * StringBuffer.cpp
 *
 *  Created on: 13 Nov 2018
 *      Author: alexsuo
 */

#include "StringBuffer.hpp"

mm::Logger mm::StringBuffer::logger;

namespace mm
{
	std::ostream& operator << (std::ostream& os, const StringBuffer& buffer)
	{
		buffer.output(os);
		return os;
	}
}


