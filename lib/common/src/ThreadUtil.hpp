/*
 * ThreadUtil.hpp
 *
 *  Created on: Oct 9, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_THREADUTIL_HPP_
#define LIB_COMMON_SRC_THREADUTIL_HPP_

#include <sched.h>

namespace mm
{
	//
	// Define the thread utility functions.
	//
	struct ThreadUtil
	{
		//
		// Set the given thread to run on the given CPU.
		//
		// pid : The thread ID.
		// cpu : THe CPU ID.
		//
		static bool setAffinity(pid_t pid, int cpu)
		{
			cpu_set_t set;
			CPU_ZERO(&set);
			CPU_SET(cpu, &set);

			return sched_setaffinity(pid, sizeof(set), &set) == 0;
		}
	};
}



#endif /* LIB_COMMON_SRC_THREADUTIL_HPP_ */
