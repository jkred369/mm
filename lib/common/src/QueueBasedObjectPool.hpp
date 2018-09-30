/*
 * QueueBasedObjectPool.hpp
 *
 *  Created on: Sep 30, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_QUEUEBASEDOBJECTPOOL_HPP_
#define LIB_COMMON_SRC_QUEUEBASEDOBJECTPOOL_HPP_

#include <moodycamel/concurrentqueue.h>

#include <NativeDefinition.hpp>

namespace mm
{
	//
	// This class implements an object pool based on a MPMC queue.
	//
	template<typename ObjectType> class QueueBasedObjectPool
	{
	public:

		// non-copyable class
		QueueBasedObjectPool(const QueueBasedObjectPool& ) = delete;
		QueueBasedObjectPool& operator = (const QueueBasedObjectPool& ) = delete;

		//
		// Constructor.
		//
		// size : The fixed size of the object pool.
		// blocking : Flag if the pool should block when empty/full.
		//
		QueueBasedObjectPool(std::size_t size, bool blocking = true) : blocking(blocking), pool(new ObjectType[size]), queue(size)
		{
			for (std::size_t i = 0; i < size; ++i)
			{
				release(&pool[i]);
			}
		}

		//
		// Destructor.
		//
		~QueueBasedObjectPool()
		{
			delete[] pool;
		}

		//
		// Estimate if the pool is empty.
		//
		// return : true if the pool looks empty.
		//
		bool empty() const
		{
			return queue.size_approx() == 0;
		}

		//
		// Get an object from the queue.
		//
		// return : The object constructed from the queue.
		//
		ObjectType* get()
		{
			ObjectType* result = nullptr;

			if (blocking)
			{
				while (!queue.try_dequeue(result));
			}
			else
			{
				queue.try_dequeue(result);
			}

			if (LIKELY(result != nullptr))
			{
				new (result) ObjectType();
			}

			return result;
		}

		//
		// Get a shared pointer pointing to a new instance retrieved from the pool.
		//
		// return : shared pointer to a new object constructed from the queue.
		//
		std::shared_ptr<ObjectType> getShared()
		{
			ObjectType* instance = get();
			if (LIKELY(instance != nullptr))
			{
				return std::shared_ptr<ObjectType> (instance, [&] (ObjectType* instance) {
					this->release(instance);
				});
			}

			return std::shared_ptr<ObjectType> ();
		}

		//
		// Release an object to the pool. The object must be previously allocated by this pool, else behavior is undefined.
		//
		// object : The object to be released.
		//
		void release(ObjectType* object)
		{
			object->~ObjectType();

			// blocking shouldn't happen here in the queue.
			queue.try_enqueue(object);
		}

	private:

		// Flag if the pool should block when its empty/full.
		const bool blocking;

		// The actual buffer
		ObjectType* const pool;

		// The internal queue for allocations.
		moodycamel::ConcurrentQueue<ObjectType*> queue;
	};
}



#endif /* LIB_COMMON_SRC_QUEUEBASEDOBJECTPOOL_HPP_ */
