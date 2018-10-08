/*
 * ObjectPool.hpp
 *
 *  Created on: Oct 30, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_LOCKFREEOBJECTPOOL_HPP_
#define LIB_COMMON_SRC_LOCKFREEOBJECTPOOL_HPP_

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <memory>

#include <NativeDefinition.hpp>

namespace mm
{
	//
	// This class implements an object pool for memory management.
	//
	template<typename ObjectType, typename Mutex = std::recursive_mutex> class LockFreeObjectPool
	{
	public:

		//
		// Node in the pool.
		//
		struct Node
		{
			// Pointer to the pool for destruction.
			LockFreeObjectPool* pool;

			// Buffer for the object.
			char objectBuffer[ sizeof(ObjectType) ];

			// The reference count.
			std::atomic<std::int64_t> refCount;

			// Pointer to next node.
			std::atomic<Node*> next;
		};

		// non-copyable class
		LockFreeObjectPool(const LockFreeObjectPool& ) = delete;
		LockFreeObjectPool& operator = (const LockFreeObjectPool& ) = delete;

		//
		// Constructor.
		//
		// size : The fixed size of the object pool.
		//
		LockFreeObjectPool(size_t size, bool blocking = true) : blocking(blocking), size(size), pool(new Node[size]), first(&pool[0])
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
		~LockFreeObjectPool()
		{
			delete[] pool;
		}

		//
		// Check if the pool is empty.
		//
		// return : true if the pool is empty.
		//
		inline bool empty() const
		{
			return first.load() == nullptr;
		}

		//
		// Get an object from the queue.
		//
		// return : The object constructed from the queue.
		//
		inline ObjectType* get()
		{
			Node* result = getNode();
			if (LIKELY(result != nullptr))
			{
				return new (result->objectBuffer) ObjectType();
			}

			return nullptr;
		}

		//
		// Get an object from the queue with the given constructor parameters.
		//
		// return : The object constructed from the queue.
		//
		template<class ... Args> inline ObjectType* get(Args... args)
		{
			Node* result = getNode();
			if (LIKELY(result != nullptr))
			{
				return new (result->objectBuffer) ObjectType(args...);
			}

			return nullptr;
		}

		//
		// Get a shared pointer pointing to a new instance retrieved from the pool.
		//
		// return : shared pointer to a new object constructed from the queue.
		//
		inline std::shared_ptr<ObjectType> getShared()
		{
			ObjectType* instance = get();
			if (LIKELY(instance != nullptr))
			{
				return std::shared_ptr<ObjectType> (instance, [] (ObjectType* instance) {
					Node* node = reinterpret_cast<Node*> (reinterpret_cast<char*> (instance) - sizeof(void*));
					node->pool->release(instance);
				});
			}

			return std::shared_ptr<ObjectType> ();
		}

		//
		// Release an object to the pool. The object must be previously allocated by this pool, else behavior is undefined.
		//
		// object : The object to be released.
		//
		inline void release(ObjectType* object)
		{
			object->~ObjectType();

			Node* node = reinterpret_cast<Node*> (reinterpret_cast<char*> (object) - sizeof(void*));
			assert(node->pool == this);

			release(node);
		}

		//
		// Release an object into the queue. The node must be originally from the buffer. Otherwise this method is a no-op.
		//
		// node : The node to be released.
		//
		inline void release(Node* node)
		{
			// sanity check
			if (UNLIKELY(node < &pool[0] || node > &pool[size - 1]))
			{
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

		//
		// Get a node from the queue.
		//
		// return : The object constructed from the queue.
		//
		inline Node* getNode()
		{
			Node* result;
			Node* next;

			while (true)
			{
				if (UNLIKELY((result = first.load()) == nullptr))
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
				if (LIKELY(result != nullptr))
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

			return result;
		}

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

		// Condition for empty
		std::condition_variable_any condition;
	};

}



#endif /* LIB_COMMON_SRC_LOCKFREEOBJECTPOOL_HPP_ */
