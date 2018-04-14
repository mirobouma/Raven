#include "Core/Threading/TaskCounter.h"
#include "Core/Threading/TaskScheduler.h"
#include "Core/Debug/LogManager.h"

namespace Raven 
{
	bool RTaskCounter::AddFiberToWaitingList(uint32_t fiberIndex, uint32_t targetValue, std::atomic<bool> *fiberStoredFlag)
	{
		RASSERT(_mTaskScheduler != nullptr);

		for (uint32_t i = 0; i < MAX_WAITING_FIBERS_ON_COUNTER; ++i) 
		{
			bool expected = true;
			
			// Try to aquire this slot.
			if (!std::atomic_compare_exchange_strong_explicit(&_mFreeSlots[i], &expected, false, std::memory_order_seq_cst, std::memory_order_relaxed)) {
				continue;
			}

			_mWaitingFibers[i].FiberIndex = fiberIndex;
			_mWaitingFibers[i].TargetValue = targetValue;
			_mWaitingFibers[i].FiberStoredFlag = fiberStoredFlag;
			_mWaitingFibers[i].InUse.store(false, std::memory_order_seq_cst);

			// Load the current value of the atomic counter and check if we already reached the counter value.
			uint32_t value = _mValue.load(std::memory_order_relaxed);
			if (_mWaitingFibers[i].InUse.load(std::memory_order_acquire)) 
			{
				return false;
			}

			if (_mWaitingFibers[i].TargetValue >= value) 
			{
				expected = false;

				// Check if another thread already took ownership.
				if (!std::atomic_compare_exchange_strong_explicit(&_mWaitingFibers[i].InUse, &expected, true, std::memory_order_seq_cst, std::memory_order_relaxed)) 
				{
					return false;
				}

				// Signal that the slot is now free.
				_mFreeSlots[i].store(true, std::memory_order_release);

				return true;
			}

			return false;
		}

		RASSERT(false);
		return false;
	}

	void RTaskCounter::CheckWaitingFibersForValue(uint32_t value)
	{
		uint32_t readyFiberIndices[MAX_WAITING_FIBERS_ON_COUNTER];
		uint32_t nextIndex = 0;

		for (uint32_t i = 0; i < MAX_WAITING_FIBERS_ON_COUNTER; ++i) 
		{
			// Check if the slot is full
			if (_mFreeSlots[i].load(std::memory_order_acquire)) {
				continue;
			}
			// Check if the slot is being modified (in use) by another thread
			if (_mWaitingFibers[i].InUse.load(std::memory_order_acquire)) {
				continue;
			}

			// Do the actual value check
			if (_mWaitingFibers[i].TargetValue >= value) {
				bool expected = false;

				// Check if another thread already took ownership.
				if (!std::atomic_compare_exchange_strong_explicit(&_mWaitingFibers[i].InUse, &expected, true, std::memory_order_seq_cst, std::memory_order_relaxed))
				{
					continue;
				}

				readyFiberIndices[nextIndex++] = i;
			}
		}
		// Exit shared section
		_mLock.fetch_sub(1u, std::memory_order_seq_cst);

		// Wait for all threads to exit the shared section if there are fibers to ready
		if (nextIndex > 0) {
			while (_mLock.load() > 0) {
				// Spin
			}

			for (uint32_t i = 0; i < nextIndex; ++i) 
			{
				// Add the fiber to the TaskScheduler's ready list
				const uint32_t index = readyFiberIndices[i];
				_mTaskScheduler->AddReadyFiber(_mWaitingFibers[index].FiberIndex, _mWaitingFibers[index].FiberStoredFlag);

				// Signal that the slot is free
				// Leave InUse == true
				_mFreeSlots[i].store(true, std::memory_order_release);
			}
		}
	}
}