/*
 * ISubscription.hpp
 *
 *  Created on: Mar 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_ISUBSCRIPTION_HPP_
#define LIB_BUS_SRC_ISUBSCRIPTION_HPP_

#include <DataType.hpp>
#include <SourceType.hpp>

namespace mm
{
	//
	// This class defines the interface for the subscription used in pub/sub structure.
	//
	template<typename Key = std::int64_t> class ISubscription
	{
	public:

		virtual ~ISubscription();

		//
		// Get the source type.
		//
		// return : The source type.
		//
		virtual SourceType sourceType() const = 0;

		//
		// Get the data type.
		//
		// return : The data type.
		//
		virtual DataType dataType() const = 0;

		//
		// Get the key.
		//
		// return : reference to const key.
		//
		virtual const Key& getKey() const = 0;
	};

	typedef ISubscription<std::int64_t> BaseSubscription;
}

#endif /* LIB_BUS_SRC_ISUBSCRIPTION_HPP_ */
