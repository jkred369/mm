/*
 * Poller.hpp
 *
 *  Created on: Feb 28, 2022
 *      Author: alexsuo
 */

#ifndef LIB_COMMON_SRC_POLLER_HPP_
#define LIB_COMMON_SRC_POLLER_HPP_

#include <IOBase.hpp>
#include <Logger.hpp>

#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>

#include <atomic>
#include <exception>
#include <string>
#include <unordered_set>

namespace mm
{
	//
	// Define a poller wrapping the epoll structure
	//
	class Poller
	{
	public:

		Poller(const std::string name = getName()) : name(name)
		{
			fd = epoll_create(1);
			if (fd < 0)
			{
				const int err = errno;
				throw std::runtime_error("Failed creating epoll fd with error = " + err);
			}
		}

		// disable copying and assignment
		Poller(const Poller& ) = delete;
		Poller& operator = (const Poller& ) = delete;

		~Poller()
		{
			if (fd > 0)
			{
				if (close(fd) != 0)
				{
					const int err = errno;
					LOGERR("Error closing epoll {} on error = {}", name, err);
				}
			}
		}

		//
		// Add the socket for the given operations.
		//
		// sock : socket concept from which a fd can be obtained.
		// operations : Flags for operations.
		//
		template<typename Sock> void add(Sock& sock, int operations)
		{
			// sanity check
			if (!((operations & READ) || (operations & WRITE)))
			{
				LOGERR("Invalid operation flag {}. Must have at least READ or WRITE.", operations);
				return;
			}

			int events = 0;
			{
				events &= EPOLLHUP;
				events &= EPOLLERR;

				if (operations & READ) events &= EPOLLIN;
				if (operations & WRITE) events &= EPOLLOUT;
			};

			const epoll_event pollEvent {events, {&sock.reactor(this), sock.fd(), 0, 0} };

			if (epoll_ctl(fd, EPOLL_CTL_ADD, sock.fd(), &pollEvent) == 0)
			{
				LOGINFO("Socket {} added to poller {}", sock.fd(), name);
			}
			else
			{
				const int err = errno;
				LOGFATAL("Failed adding socket {} to poller {} on err {}", sock.fd(), name, err);
			}
		}

		//
		// Remove the given socket from the poller.
		//
		// socket : The socket to remove.
		//
		template<typename Sock> void remove(Sock& sock)
		{
			if (epoll_ctl(fd, EPOLL_CTL_DEL, sock.fd(), nullptr) == 0)
			{
				LOGINFO("Socket {} removed from poller {}", sock.fd(), name);
			}
			else
			{
				const int err = errno;
				LOGFATAL("Failed removing socket {} from poller {} on err {}", sock.fd(), name, err);
			}
		}

		//
		// Run the polling and call back logic for 1 full round.
		//
		void poll()
		{
			epoll_event events[20];

			const int count = epoll_wait(fd, events, sizeof(events), -1);
			if (UNLIKELY(count < 0))
			{
				const int err = errno;
				LOGERR("Error polling on poller {}, err = {}", name, err);

				return;
			}

			for (int i = 0; i < count; ++i)
			{
				(*events[i].data.ptr)(events[i].events);
			}
		}

		template<typename Sock> void scheduleForSend(Sock& sock)
		{
			const int events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
			const epoll_event pollEvent {events, {&sock.reactor(this), sock.fd(), 0, 0} };

			if (epoll_ctl(fd, EPOLL_CTL_MOD, sock.fd(), events) != 0)
			{
				const int err = errno;
				LOGFATAL("Failed schedule socket {} on poller {} with err {}", sock.fd(), name, err);
			}
		}

	private:

		// logger for poller.
		static Logger logger;

		//
		// Get a default name of the poller.
		//
		// return : A default name
		//
		static std::string getName()
		{
			static std::string Name = "Poller-";
			static std::atomic<std::int64_t> Id {0};

			return Name + (++Id);
		}

		// FD of the poller.
		const int fd;

		// name of the poller.
		const std::string name;
	};
}



#endif /* LIB_COMMON_SRC_POLLER_HPP_ */
