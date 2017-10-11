/*
 * Scheduler.hpp
 *
 *  Created on: Oct 11, 2017
 *      Author: suoalex
 */

#ifndef LIB_SYSTEM_SRC_SCHEDULER_HPP_
#define LIB_SYSTEM_SRC_SCHEDULER_HPP_

namespace mm
{
	//
	// The scheduler working together with dispatcher.
	//
	template<typename Key = std::int32_t, typename Dispatcher> class Scheduler
	{
	public:

		//
		// Constructor.
		//
		// dispatcher : The dispatcher which will execute the actual task.
		//
		Scheduler(Dispatcher& dispatcher) : dispatcher(dispatcher)
		{
		}

	private:

		// The dispatcher used.
		Dispatcher& dispatcher;
	};
}



#endif /* LIB_SYSTEM_SRC_SCHEDULER_HPP_ */
