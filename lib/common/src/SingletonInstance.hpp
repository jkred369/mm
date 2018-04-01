/*
 * SingletonInstance.hpp
 *
 *  Created on: Mar 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_SINGLETONINSTANCE_HPP_
#define LIB_COMMON_SRC_SINGLETONINSTANCE_HPP_

#include <functional>
#include <unordered_set>

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <sfinae.hpp>

namespace mm
{
	//
	// This class defines a singleton instance set where each constructed instance must be a singleton
	// within the same process.
	//
	// The class requires its instances to have a proper hash function implemented.
	//
	template<
		typename ValueType,
		typename Hash = std::hash<ValueType>
	> class SingletonInstance
	{
	public:

		//
		// Constructor.
		//
		// size : The default size of the internal set.
		//
		SingletonInstance(size_t size = 8) : INSTANCES(size)
		{
			static_assert(sfinae::has_equal<ValueType>::value, "VaueType must implement == operator.");
		}

		//
		// Get the singleton instance for the value type class on the process.
		//
		// In case the value is requested the first time, create and store it.
		//
		// params : The parameter sets for the constructor of the value type.
		//
		// return : The singleton for the value type given the params for its constructor.
		//
		template<typename ... Params> static const ValueType& getInstance(Params ... params)
		{
			ValueType value(&params...);

			{
				boost::upgrade_lock<boost::shared_mutex> lock(MUTEX);

				// insert if not found
				std::unordered_set<ValueType>::iterator it = INSTANCES.find(value);
				if (it == INSTANCES.end())
				{
					boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

					INSTANCES.insert(value);
					it = INSTANCES.find(value);
				}

				return *it;
			}
		}

	private:

		// The set of instances.
		static std::unordered_set<ValueType> INSTANCES;

		// The mutex for accessing the instances.
		static boost::shared_mutex MUTEX;
	};
}



#endif /* LIB_COMMON_SRC_SINGLETONINSTANCE_HPP_ */
