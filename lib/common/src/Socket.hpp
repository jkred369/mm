/*
 * Socket.hpp
 *
 *  Created on: Feb 28, 2022
 *      Author: alexsuo
 */

#ifndef LIB_COMMON_SRC_SOCKET_HPP_
#define LIB_COMMON_SRC_SOCKET_HPP_

#include <IOReactor.hpp>
#include <Logger.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>
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

		// Supported socket types
		static constexpr int TCP = 0;
		static constexpr int UDP = 1;

		//
		// Constructor.
		//
		// type : The type of socket.
		// name : name of the socket.
		//
		Socket(int type = TCP, std::string name = getName()) :
			type(type),
			socketName(name),
			reactor( std::bind(&Socket::process, this) )
		{
		}

		// disable copying
		Socket(const Socket&) = delete;

		//
		// Destructor will close the socket if any.
		//
		~Socket()
		{
			close();
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
			return sockfd;
		}

		//
		// Connect to the given address and port.
		//
		bool connect(const char* address, int port)
		{
			const int actualType = type == UDP ? SOCK_DGRAM : SOCK_STREAM;
			const int protocol = type == UDP ? udpProtocol() : tcpProtocol();

			sockfd = socket(AF_INET, actualType | SOCK_NONBLOCK, protocol);
			if (sockfd < 0)
			{
				const int err = errno;
				LOGERR("Failed opening socket for {}, err = {}", name, err);

				return false;
			}

			sockaddr_in addr;
			{
				std::memset(&addr, 0, sizeof(addr));
				addr.sin_addr = inet_addr(address);
				addr.sin_family = AF_INET;
				addr.sin_port = port;
			}

			if (::connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
			{
				const int err = errno;
				LOGERR("Failed connecting socket for {}, err = {}", name, err);

				return false;
			}

			return true;
		}

		//
		// Close the socket.
		//
		void close()
		{
			if (sockfd > 0)
			{
				if (::close(sockfd) != 0)
				{
					const int err = errno;
					LOGERR("Error closing socket {}, err = {}", sockfd, err);

					sockfd = 0;
				}
			}
		}

		//
		// Process the events - both read and write.
		//
		void process(uint32_t events)
		{

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

		static int tcpProtocol()
		{
			static int tcp = getprotobyname("tcp")->p_proto;
			return tcp;
		}

		static int udpProtocol()
		{
			static int udp = getprotobyname("udp")->p_proto;
			return udp;
		}

		// Logger of the socket.
		static Logger logger;

		// Type of the socket.
		const int type;

		// Name of the socket
		const std::string socketName;

		// The reactor.
		const Reactor reactor;

		// The actual fd.
		int sockfd {0};
	};
}



#endif /* LIB_COMMON_SRC_SOCKET_HPP_ */
