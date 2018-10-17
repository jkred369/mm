/*
 * ThreadUtil.hpp
 *
 *  Created on: Oct 9, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_THREADUTIL_HPP_
#define LIB_COMMON_SRC_THREADUTIL_HPP_

#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace mm
{
	//
	// Define the thread utility functions.
	//
	struct ThreadUtil
	{
		// Flag the CPU affinity ID isn't set.
		static constexpr int CPU_ID_NOT_SET = -1;

		//
		// Set the current thread to run on the given CPU.
		//
		// cpu : THe CPU ID.
		//
		static bool setAffinity(int cpu)
		{
			const pid_t pid = syscall(SYS_gettid);
			return setAffinity(pid, cpu);
		}

		//
		// Set the given thread to run on the given CPU.
		//
		// pid : The thread ID.
		// cpu : THe CPU ID.
		//
		static bool setAffinity(pid_t pid, int cpu)
		{
			if (cpu == CPU_ID_NOT_SET)
			{
				return false;
			}

			cpu_set_t set;
			CPU_ZERO(&set);
			CPU_SET(cpu, &set);

			const int result = sched_setaffinity(pid, sizeof(set), &set);
			return result == 0;
		}
	};
}



#endif /* LIB_COMMON_SRC_THREADUTIL_HPP_ */
