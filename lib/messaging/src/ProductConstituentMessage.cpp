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
	Constituent::Constituent() : id(0), weight(0.0)
	{
	}

	bool Constituent::equals(const Constituent& rhs) const
	{
		return id == rhs.id && weight == rhs.weight;
	}

	ProductConstituentMessage::ProductConstituentMessage() : id(0), count(0)
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


