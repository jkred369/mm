/*
 * Socket.hpp
 *
 *  Created on: Feb 28, 2022
 *      Author: alexsuo
 */

#ifndef LIB_COMMON_SRC_SOCKET_HPP_
#define LIB_COMMON_SRC_SOCKET_HPP_

#include <Runnable.hpp>

#include <sys/socket.h>

#include <functional>
#include <string>

namespace mm
{
	//
	// This class implements a UDP or TCP socket connection and realize a Sock concept used in poller.
	//
	class Socket
	{
	public:

		//
		// Constructor.
		//
		// name : name of the socket.
		//
		Socket(std::string name = getName()) : socketName(name)
		{
		}

		// disable copying
		Socket(const Socket&) = delete;

		~Socket()
		{
			if (fd != 0)
			{

			}
		}

		//
		// Get the name of the socket.
		//
		// return : Name of the socket.
		//
		const std::string& name() const
		{
			return socketName;
		}

		//
		// Get the fd of the socket.
		//
		// return : The platform specific fd.
		//
		inline int fd() const
		{
			return descriptor;
		}

	private:

		//
		// Get a default name of the poller.
		//
		// return : A default name
		//
		static std::string getName()
		{
			static std::string Name = "Socket-";
			static std::atomic<std::int64_t> Id {0};

			return Name + (++Id);
		}

		// Name of the socket
		const std::string socketName;

		// The reactor.
		const Runnable reactor;

		// The actual fd.
		int descriptor {0};
	};
}



#endif /* LIB_COMMON_SRC_SOCKET_HPP_ */
