/*
 * NullObjectPool.hpp
 *
 *  Created on: Sep 30, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_NULLOBJECTPOOL_HPP_
#define LIB_COMMON_SRC_NULLOBJECTPOOL_HPP_

#include <cstdint>
#include <memory>

namespace mm
{
	//
	// This class implements an "object pool" which only does system calls.
	//
	template<typename ObjectType> class NullObjectPool
	{
	public:

		// non-copyable class
		NullObjectPool(const NullObjectPool& ) = delete;
		NullObjectPool& operator = (const NullObjectPool& ) = delete;

		//
		// Constructor.
		//
		// size : The fixed size of the object pool.
		// blocking : Flag if the pool should block when empty/full.
		//
		NullObjectPool(std::size_t size, bool blocking = true)
		{
		}

		//
		// Estimate if the pool is empty.
		//
		// return : always false.
		//
		inline bool empty() const
		{
			return false;
		}

		//
		// Get an object from the queue.
		//
		// return : The object constructed from the queue.
		//
		inline ObjectType* get()
		{
			return new ObjectType();
		}

		//
		// Get a shared pointer pointing to a new instance retrieved from the pool.
		//
		// return : shared pointer to a new object constructed from the queue.
		//
		inline std::shared_ptr<ObjectType> getShared()
		{
			return std::make_shared<ObjectType> ();
		}

		//
		// Release an object to the pool.
		//
		// object : The object to be released.
		//
		inline void release(ObjectType* object)
		{
			delete object;
		}
	};

}



#endif /* LIB_COMMON_SRC_NULLOBJECTPOOL_HPP_ */
