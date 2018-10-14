/*
 * IdGenerator.hpp
 *
 *  Created on: Oct 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_IDGENERATOR_HPP_
#define LIB_COMMON_SRC_IDGENERATOR_HPP_

#include <mutex>

namespace mm
{
	//
	// This class implement ID generator concept by providing a function with signature
	//
	// std::int64_t generate()
	//
	// since the generator is on critical path we don't use virtual function.
	//
	template<typename Mutex = std::mutex> class NumericalIdGenerator
	{
	public:

		//
		// Constructor.
		//
		// prefix : The prefix number.
		// shiftFactor : The shift factor (i.e. maximum count of IDs to generate)
		//
		NumericalIdGenerator(std::int64_t prefix, std::int64_t shiftFactor) : prefix(prefix), shiftFactor(shiftFactor), counter(0)
		{
		}

		//
		// Generate the ID.
		//
		// return : The new generated ID as prefix * shiftFacor + incremented counter.
		//
		std::int64_t generate()
		{
			std::unique_lock<Mutex> lock(mutex);
			return prefix * shiftFactor + (++counter);
		}

	private:

		// Prefix of the ID.
		const std::int64_t prefix;

		// Shift factor aka the max of ID generated.
		const std::int64_t shiftFactor;

		// Counter for increment.
		std::int64_t counter;

		// The mutex for any thread safety.
		Mutex mutex;
	};
}



#endif /* LIB_COMMON_SRC_IDGENERATOR_HPP_ */
