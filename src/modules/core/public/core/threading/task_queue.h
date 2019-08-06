#pragma once

#include <atomic>
#include <memory>
#include <vector>
#include "core/debug/log_manager.h"

/**
* This is an implementation of 'Correct and Efficient Work-Stealing for Weak Memory Models' by Le et. al [2013]
*
* https://hal.inria.fr/hal-00802885
*/

/** Retrieved from https://github.com/RichieSams/FiberTaskingLib/blob/master/include/ftl/wait_free_queue.h */

namespace Raven 
{

	template<typename T>
	class RTaskQueue {
	public:
		RTaskQueue()
			: _mTop(1),
			_mBottom(1),
			_mArray(new FCircularArray(32)) {
		}
		~RTaskQueue() {
			delete _mArray.load(std::memory_order_relaxed);
		}

	private:
		class FCircularArray {
		public:
			FCircularArray(std::size_t n)
				: items(n) 
			{
				RASSERT((!(n == 0) && !(n & (n - 1)))); // must be power of two.
			}

		private:
			std::vector<T> items;
			std::unique_ptr<FCircularArray> previous;

		public:
			std::size_t Size() const {
				return items.size();
			}

			T Get(std::size_t index) {
				return items[index & (Size() - 1)];
			}

			void Put(std::size_t index, T x) {
				items[index & (Size() - 1)] = x;
			}

			// Growing the array returns a new circular_array object and keeps a
			// linked list of all previous arrays. This is done because other threads
			// could still be accessing elements from the smaller arrays.
			FCircularArray *Grow(std::size_t top, std::size_t bottom) 
			{
				FCircularArray *new_array = new FCircularArray(Size() * 2);
				new_array->previous.reset(this);
				for (std::size_t i = top; i != bottom; i++) {
					new_array->Put(i, Get(i));
				}
				return new_array;
			}
		};

		std::atomic<uint64_t> _mTop; /*!< Top of the circular array. */

		// Cache-line pad
		char pad[64];

		std::atomic<uint64_t> _mBottom; /*!< Bottom of the circular array. */

		// Cache-line pad
		char pad2[64];

		std::atomic<FCircularArray*> _mArray;


	public:
		void Push(T value)
		{
			uint64_t b = _mBottom.load(std::memory_order_relaxed);
			uint64_t t = _mTop.load(std::memory_order_acquire);

			FCircularArray* array = _mArray.load(std::memory_order_relaxed);

			if (b - t > array->Size() - 1) {
				/* Full queue. */
				array = array->Grow(t, b);
				_mArray.store(array, std::memory_order_release);
			}
			array->Put(b, value);

			std::atomic_thread_fence(std::memory_order_release);
			_mBottom.store(b + 1, std::memory_order_relaxed);
		}

		bool Pop(T *value) 
		{
			uint64_t b = _mBottom.load(std::memory_order_relaxed) - 1;
			FCircularArray* array = _mArray.load(std::memory_order_relaxed);
			_mBottom.store(b, std::memory_order_relaxed);

			std::atomic_thread_fence(std::memory_order_seq_cst);

			uint64_t t = _mTop.load(std::memory_order_relaxed);
			bool result = true;
			if (t <= b) {
				/* Non-empty queue. */
				*value = array->Get(b);
				if (t == b) {
					/* Single last element in queue. */
					if (!std::atomic_compare_exchange_strong_explicit(&_mTop, &t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
						/* Failed race. */
						result = false;
					}
					_mBottom.store(b + 1, std::memory_order_relaxed);
				}
			}
			else {
				/* Empty queue. */
				result = false;
				_mBottom.store(b + 1, std::memory_order_relaxed);
			}

			return result;
		}

		bool Steal(T *value) 
		{
			uint64_t t = _mTop.load(std::memory_order_acquire);
			std::atomic_thread_fence(std::memory_order_seq_cst);
			uint64_t b = _mBottom.load(std::memory_order_acquire);

			if (t < b) 
			{
				/* Non-empty queue. */
				FCircularArray* array = _mArray.load(std::memory_order_consume);
				*value = array->Get(t);
				if (!std::atomic_compare_exchange_strong_explicit(&_mTop, &t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) 
				{
					/* Failed race. */
					return false;
				}

				return true;
			}

			return false;
		}
	};
}