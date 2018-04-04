#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

namespace Raven
{
	/*! \class RAtomicCounter
	\brief Atomic counter to keep track of completed coroutine tasks. 
	*/
	class RAtomicCounter 
	{
	public:
		RAtomicCounter(uint32_t initialValue = 0) : _mValue(initialValue) {}

		/*! A wrapper over std::atomic_uint32_t::load()
		\param memoryOrder    Memory order to use when loading the counter value.
		\return               The current value of the counter
		*/
		inline uint32_t Load(std::memory_order memoryOrder = std::memory_order_seq_cst) {
			return _mValue.load(memoryOrder);
		}

		/*! A wrapper over std::atomic_uint32_t::store()
		\param x              The value to load into the counter
		\param memoryOrder    The memory order to use for the store
		*/
		inline void Store(uint32_t x, std::memory_order memoryOrder = std::memory_order_seq_cst) {
			_mValue.store(x, memoryOrder);
		}

		/*! A wrapper over std::atomic_uint32_t::fetch_add()
		\param x              The value to add to the counter
		\param memoryOrder    The memory order to use for the fetch_add
		\return               The value of the counter before the addition
		*/
		inline uint32_t FetchAdd(uint32_t x, std::memory_order memoryOrder = std::memory_order_seq_cst) {
			return _mValue.fetch_add(x, memoryOrder);
		}

		/*! A wrapper over std::atomic_uint32_t::fetch_sub()
		\param x              The value to subtract from the counter
		\param memoryOrder    The memory order to use for the fetch_sub
		\return               The value of the counter before the subtraction
		*/
		inline uint32_t FetchSub(uint32_t x, std::memory_order memoryOrder = std::memory_order_seq_cst) 
		{
			return _mValue.fetch_sub(x, memoryOrder);
		}

	protected:
		/*! Internal counter value. */
		std::atomic_uint32_t _mValue;
	};
}