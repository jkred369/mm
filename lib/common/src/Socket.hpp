/*
 * Socket.hpp
 *
 *  Created on: Feb 28, 2022
 *      Author: alexsuo
 */

#ifndef LIB_COMMON_SRC_SOCKET_HPP_
#define LIB_COMMON_SRC_SOCKET_HPP_

#include <IOBase.hpp>
#include <Logger.hpp>
#include <Poller.hpp>

#include <sys/epoll.h>
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
	// The socket listener interface.
	//
	struct SocketListener
	{
		virtual ~SocketListener() {}

		virtual void onData(const char* buffer, int size);
	};

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

		// disable copying and assignment
		Socket(const Socket&) = delete;
		Socket& operator = (const Socket& ) = delete;

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

		void setListener(SocketListener* listener)
		{
			this->listener = listener;
		}

		SocketListener* getListener() const
		{
			return listener;
		}

		const Reactor& reactor(Poller* poller)
		{
			eventReactor = std::bind(&Socket::process, this, poller);
			return eventReactor;
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
		void process(Poller* poller, uint32_t events)
		{
			if (events | EPOLLOUT)
			{
				send();
			}

			if (events | EPOLLIN)
			{
				recv();
			}

			if (events | EPOLLHUP)
			{
				LOGERR("Socket {}, {} got hang up.", socketName, sockfd);
			}
		}

		void send()
		{

		}

		void send(const char* data, size_t size)
		{
			const char* curr = data;

			for (size_t curSize = size; size > 0; )
			{
				const ssize_t result = ::send(sockfd, curr, curSize, MSG_DONTWAIT);
				if (result > 0)
				{
					if (result == curSize)
					{
						// job done all sent
						break;
					}
					else if (result < curSize)
					{
						// next block
						curSize -= result;
						curr += result;
					}
					else
					{
						// error
						LOGERR("Send buffer overflow, sent {}, buffer length {}", result, curSize);
						break;
					}
				}
				else
				{
					const int err = errno;
					if ((err & EAGAIN) || (err & EWOULDBLOCK))
					{
						// adding this socket to epoll for out event
					}
					else
					{
						LOGERR("Error sending data on socket {} with err = {}", name, err);
						break;
					}
				}
			}
		}

		void recv()
		{
			const int size = ::recv(sockfd, buffer, sizeof(buffer), MSG_DONTWAIT);
			if (UNLIKELY(size < 0))
			{
				const int err = errno;
				LOGERR("Error reading socket {}, err = {}", sockfd, err);

				return;
			}

			if (size && listener)
			{
				listener->onData(buffer, size);
			}
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
		const Reactor eventReactor;

		// The actual fd.
		int sockfd {0};

		// The receive buffer
		char buffer[8192];

		// The socket listener
		SocketListener * listener {nullptr};

	};
}



#endif /* LIB_COMMON_SRC_SOCKET_HPP_ */
