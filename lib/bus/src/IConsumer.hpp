/*
 * IConsumer.hpp
 *
 *  Created on: Mar 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_ICONSUMER_HPP_
#define LIB_BUS_SRC_ICONSUMER_HPP_

#include <memory>

namespace mm
{
	//
	// This class defines interface for the message consumer.
	//
	template<typename Message> class IConsumer
	{
	public:

		// Virtual destructor.
		virtual ~IConsumer()
		{
		}

		//
		// Consume the message.
		//
		// message : The given message to be consumed.
		//
		virtual void consume(const std::shared_ptr<const Message>& message) = 0;
	};
}



#endif /* LIB_BUS_SRC_ICONSUMER_HPP_ */
