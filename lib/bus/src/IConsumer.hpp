/*
 * IConsumer.hpp
 *
 *  Created on: Mar 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_ICONSUMER_HPP_
#define LIB_BUS_SRC_ICONSUMER_HPP_

#include <memory>

#include <Message.hpp>

namespace mm
{
	class IConsumer
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
		virtual void consume(const std::shared_ptr<Message> message);
	};
}



#endif /* LIB_BUS_SRC_ICONSUMER_HPP_ */
