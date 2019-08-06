#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#define MAX_WAITING_FIBERS_ON_COUNTER 16

namespace Raven
{
	class RTaskScheduler;

	/*! \class FWaitingFiber
	\brief Structure defining a fiber which is waiting for a certain value.
	*/
	struct FWaitingFiber {
		FWaitingFiber()
			: InUse(true),
			FiberIndex(0),
			TargetValue(0),
			FiberStoredFlag(nullptr)
		{
		}

		/*! A bundle is "InUse" when it's being filled with data, or when it's being removed
		The wait checking code uses this atomic to prevent multiple threads from modifying the same slot */
		std::atomic<bool> InUse;

		/*! The index of the fiber that is waiting on this counter */
		std::size_t FiberIndex;

		/*! The value the fiber is waiting for */
		uint32_t TargetValue;

		/*! A flag signaling if the fiber has been successfully switched out of and "cleaned up" */
		std::atomic<bool> *FiberStoredFlag;
	};

	/*! \class RTaskCounter
	\brief Atomic counter to keep track of completed coroutine tasks. 
	*/
	class RTaskCounter
	{
	public:
		RTaskCounter(uint32_t initialValue = 0) : _mValue(initialValue) 
		{
			for (uint32_t i = 0; i < MAX_WAITING_FIBERS_ON_COUNTER; ++i) 
			{
				_mFreeSlots[i].store(true);
				_mWaitingFibers[i].InUse.store(true);
			}

			_mLock.store(0);
		}

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
			_mLock.fetch_add(1u, std::memory_order_seq_cst);
			_mValue.store(x, memoryOrder);
			CheckWaitingFibersForValue(x);
		}

		/*! A wrapper over std::atomic_uint32_t::fetch_add()
		\param x              The value to add to the counter
		\param memoryOrder    The memory order to use for the fetch_add
		\return               The value of the counter before the addition
		*/
		inline uint32_t FetchAdd(uint32_t x, std::memory_order memoryOrder = std::memory_order_seq_cst) {
			_mLock.fetch_add(1u, std::memory_order_seq_cst);
			const uint32_t old = _mValue.fetch_add(x, memoryOrder);
			CheckWaitingFibersForValue(old + x);

			return old;
		}

		/*! A wrapper over std::atomic_uint32_t::fetch_sub()
		\param x              The value to subtract from the counter
		\param memoryOrder    The memory order to use for the fetch_sub
		\return               The value of the counter before the subtraction
		*/
		inline uint32_t FetchSub(uint32_t x, std::memory_order memoryOrder = std::memory_order_seq_cst) 
		{
			_mLock.fetch_add(1u, std::memory_order_seq_cst);
			const uint32_t old = _mValue.fetch_sub(x, memoryOrder);
			CheckWaitingFibersForValue(old - x);

			return old;
		}

	protected:
		friend class RTaskScheduler;

		/*! Adds a specific fiber to the waiting list of this counter. */
		bool AddFiberToWaitingList(uint32_t fiberIndex, uint32_t targetValue, std::atomic<bool> *fiberStoredFlag);

		/*! Checks if any of the fibers that are waiting for a specific 
		value can continue execution, and adds them to the task scheduler.*/
		void CheckWaitingFibersForValue(uint32_t value);

		/*! Internal counter value. */
		std::atomic_uint32_t _mValue;

		/*! Internal lock atomic value. */
		std::atomic_uint32_t _mLock;

		/* The TaskScheduler this counter is associated with */
		RTaskScheduler* _mTaskScheduler;

		/*! Array of atomic bools describing if a certain slot of waiting fibers is free. */
		std::atomic<bool> _mFreeSlots[MAX_WAITING_FIBERS_ON_COUNTER];
		FWaitingFiber	  _mWaitingFibers[MAX_WAITING_FIBERS_ON_COUNTER];
	};
}