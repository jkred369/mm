/*
 * FieldDefinition.hpp
 *
 *  Created on: Sep 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_FIELDDEFINITION_HPP_
#define LIB_MESSAGING_SRC_FIELDDEFINITION_HPP_

#include <cstdint>
#include <chrono>

#include <DateTime.hpp>
#include <FixedSizeString.hpp>

namespace mm
{
	// defines all the field types for specific fields.
	typedef FixedSizeString<31> SymbolType;
	typedef FixedSizeString<21> ExecutionIdType;
}



#endif /* LIB_MESSAGING_SRC_FIELDDEFINITION_HPP_ */
