/*
 * ProductConstituentMessage.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include <algorithm>

#include "ProductConstituentMessage.hpp"

namespace mm
{
	ProductConstituentMessage::ProductConstituentMessage() : instrumentId(0), count(0)
	{
	}

	bool ProductConstituentMessage::equals (const ProductConstituentMessage& rhs) const
	{
		if (id != rhs.id || count != rhs.count)
		{
			return false;
		}

		for (const Constituent& c : constituents)
		{
			if (std::find(rhs.constituents.begin(), rhs.constituents.end(), c) == rhs.constituents.end())
			{
				return false;
			}
		}

		return true;
	}

}


