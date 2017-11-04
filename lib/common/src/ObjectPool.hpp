/*
 * ObjectPool.hpp
 *
 *  Created on: Oct 30, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_OBJECTPOOL_HPP_
#define LIB_COMMON_SRC_OBJECTPOOL_HPP_

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <memory>

#include "NativeDefinition.hpp"

namespace mm
{
	//
	// This class implements an object pool for memory management.
	//
	template<typename ObjectType, typename Mutex = std::recursive_mutex> class ObjectPool
	{
	public:

		//
		// Node in the pool.
		//
		struct Node
		{
			// Pointer to the pool for destruction.
			ObjectPool* pool;

			// Buffer for the object.
			char objectBuffer[ sizeof(ObjectType) ];

			// Pointer to next node.
			std::atomic<Node*> next;
		};

		// non-copyable class
		ObjectPool(const ObjectPool& ) = delete;
		ObjectPool& operator = (const ObjectPool& ) = delete;

		//
		// Constructor.
		//
		// size : The fixed size of the object pool.
		//
		ObjectPool(size_t size, bool blocking = true) : blocking(blocking), size(size), pool(new Node[size]), first(&pool[0])
		{
			for (size_t i = 0; i < size - 1; ++i)
			{
				pool[i].next.store(&pool[i + 1]);
			}

			pool[size - 1].next.store(nullptr);
		}

		//
		// Destructor. Deallocate the pool.
		//
		~ObjectPool()
		{
			delete[] pool;
		}

		//
		// Check if the pool is empty.
		//
		// return : true if the pool is empty.
		//
		bool empty()
		{
			return first.load() == nullptr;
		}

		//
		// Get an object from the queue.
		//
		// return : The object constructed from the queue.
		//
		ObjectType* get()
		{
			Node* result;
			Node* next;

			while (true)
			{
				if ((result = first.load()) == nullptr)
				{
					if (blocking)
					{
						std::unique_lock<Mutex> lock(mutex);
						condition.wait(lock);
					}
					else
					{
						return nullptr;
					}
				}

				// must check here in case we back from condition signal
				if (result != nullptr)
				{
					next = result->next.load();
					if (first.compare_exchange_weak(result, next, std::memory_order_release, std::memory_order_relaxed))
					{
						break;
					}
				}
			}

			result->pool = this;
			result->next.store(nullptr);

			return new (result->objectBuffer) ObjectType();
		}

		//
		// Release an object to the pool. The object must be previously allocated by this pool, else behavior is undefined.
		//
		// object : The object to be released.
		//
		void release(ObjectType* object)
		{
			Node* node = reinterpret_cast<Node*> (reinterpret_cast<char*> (object) - sizeof(void*));
			assert(node->pool == this);

			release(node);
		}

	protected:

		//
		// Release an object into the queue. The node must be originally from the buffer. Otherwise this method is a no-op.
		//
		// node : The node to be released.
		//
		void release(Node* node)
		{
			// sanity check
			if (node < &pool[0] || node > &pool[size - 1])
			{
				delete node;
				return;
			}

			// make sure the fields are correct.
			node->pool = this;

			// insertion
			Node* firstValue;
			while (true)
			{
				firstValue = first.load();
				node->next.store(firstValue);

				// move the last then append
				if (first.compare_exchange_weak(firstValue, node, std::memory_order_release, std::memory_order_relaxed))
				{
					break;
				}
			}

			// signal where needed
			if (blocking && node == first.load())
			{
				condition.notify_all();
			}
		}

	private:

		// Flag if this pool should block on empty.
		const bool blocking;

		// Size of the object pool.
		const size_t size;

		// The pool of objects.
		Node* const pool;

		// Pointer to the first node available.
		std::atomic<Node*> first;

		// Lock for blocking if needed
		Mutex mutex;

		// Conditoin for empty
		std::condition_variable_any condition;
	};

}



#endif /* LIB_COMMON_SRC_OBJECTPOOL_HPP_ */
