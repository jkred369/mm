/*
 * IPublisher.hpp
 *
 *  Created on: Mar 13, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_IPUBLISHER_HPP_
#define LIB_BUS_SRC_IPUBLISHER_HPP_

#include <Message.hpp>

namespace mm
{
	//
	// The publisher defines the interface for sending a message.
	//
	class IPublisher
	{
	public:

		virtual ~IPublisher();

		//
		// Publish a message to the bus.
		//
		// message : The message to be published.
		//
		virtual void publish(const std::string& topic, const Message& message) = 0;
	};
}


#endif /* LIB_BUS_SRC_IPUBLISHER_HPP_ */
