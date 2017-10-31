/*
 * ObjectPool.hpp
 *
 *  Created on: Oct 30, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_OBJECTPOOL_HPP_
#define LIB_COMMON_SRC_OBJECTPOOL_HPP_

#include <atomic>
#include <mutex>
#include <memory>

namespace mm
{
	//
	// This class implements an object pool for memory management.
	//
	template<typename ObjectType> class ObjectPool
	{
	public:

		//
		// The class as root class for all object in the pool.
		//
		class Recyclable
		{
			// non-copyable class
			Recyclable(const Recyclable& ) = delete;
			Recyclable& operator = (const Recyclable& ) = delete;

			//
			// Destructor - put the object back to the pool.
			//
			virtual ~Recyclable()
			{

			}
		};

		// non-copyable class
		ObjectPool(const ObjectPool& ) = delete;
		ObjectPool& operator = (const ObjectPool& ) = delete;

		//
		// Constructor.
		//
		// size : The fixed size of the object pool.
		//
		ObjectPool(size_t size) : size(size), pool(new Node[size]), first(pool[0]), last(pool[size - 1])
		{
			for (size_t i = 0; i < size; ++i)
			{
				pool[i].next.store(pool[i + 1]);
			}
		}

		//
		// Destructor. Deallocate the pool.
		//
		~ObjectPool()
		{
			delete[] pool;
		}

		//
		// Get an object from the queue.
		//
		ObjectType* get()
		{
			Node* result;

			while (true)
			{
				result = first;

				if (first.compare_exchange_weak(first, first->next.load(),
						std::memory_order_release, std::memory_order_relaxed))
				{
					break;
				}
			}

			result.next.store(nullptr);
			return new (result.objectBuffer) ObjectType();
		}

	protected:

		//
		// Node in the pool.
		//
		template<typename ObjectType> struct Node
		{
			// Pointer to the pool for destruction.
			ObjectPool* pool;

			// Buffer for the object.
			char objectBuffer[ sizeof (ObjectType) ];

			// Pointer to next node.
			std::atomic<Node*> next;
		};

	private:

		// Size of the object pool.
		size_t size;

		// The pool of objects.
		Node* const pool;

		// Pointer to the first node available.
		std::atomic<Node*> first;

		// Pointer to the last node.
		std::atomic<Node*> last;
	};
}



#endif /* LIB_COMMON_SRC_OBJECTPOOL_HPP_ */
