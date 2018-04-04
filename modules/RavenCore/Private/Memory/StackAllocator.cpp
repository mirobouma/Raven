#include "Core/Memory/StackAllocator.h"
#include "Core/Memory/MemoryUtils.h"
#include "HAL/PlatformInfo.h"
#include "Core/Debug/LogManager.h"
#include <cstring>

namespace Raven 
{
	RStackAllocator::RStackAllocator(uint32_t stackSize)
	{
		if (stackSize == UINT32_MAX)
		{
			// Use system defined page size.
			stackSize = RPlatformInfo::GetPageSize();
		}

		mStackSize = stackSize;
		mMemory = malloc(stackSize);
		mCurrentOffset = 0;

		mLastAllocation.allocationSize = 0;
		mLastAllocation.memoryOffset = 0;
		mLastAllocation.ptr = nullptr;
	}

	RStackAllocator::~RStackAllocator()
	{
		if (mMemory != nullptr)
		{
			// free our stack memory.
			free(mMemory);
		}
	}

	void RStackAllocator::Reset(bool freeMemory)
	{
		// If we want to free all previously allocated blocks, clear the mMemBlocks vector. 
		// the unique ptr should automatically call our deconstructor which will free it's underlying memory.
		if (freeMemory)
		{
			free(mMemory);
			mMemory = nullptr;
		}
		
		mCurrentOffset = 0;
	}

	FStackAllocation RStackAllocator::Allocate(uint32_t size, uint32_t alignment)
	{
		FStackAllocation alloc;

		const size_t AlignmentMask = alignment - 1;

		// Assert that it's a power of two and larger than the size of a pointer.
		RASSERT((alignment >= sizeof(void*)));
		RASSERT((AlignmentMask & alignment) == 0);

		// We could store the header in the alignment but our aligned malloc implementation might use this space, so we just store it at the beginning of our block.
		uint32_t alignedOffset = Memory::IsAligned(mCurrentOffset, alignment) ? mCurrentOffset : Memory::AlignUp(mCurrentOffset, alignment);
		uint32_t alignedSize = Memory::AlignUp(size + sizeof(FStackHeader), alignment);

		if (alignedOffset + alignedSize > mStackSize)
		{
			RLOG_ERROR("Exceeded stack allocation size, increase the maximum stack size.");
			memset(&alloc, 0, sizeof(FStackAllocation));
			return alloc;
		}

		// Define allocation, store header.
		alloc.allocationSize = alignedSize - sizeof(FStackHeader);
		alloc.memoryOffset = alignedOffset + sizeof(FStackHeader);
		alloc.ptr = (uint32_t*)mMemory + alloc.memoryOffset;

		// Find the location of the header and fill it with information.
		FStackHeader* header = (FStackHeader*)((uint32_t*)mMemory + alloc.memoryOffset - sizeof(FStackHeader));
		header->prevAllocationSize = mLastAllocation.allocationSize;

		// Copy the current allocation into the previous allocation reference.
		memcpy(&mLastAllocation, &alloc, sizeof(FStackAllocation));

		// Reserve this memory for the new allocation in the block.
		mCurrentOffset = alignedOffset + alignedSize;

		return alloc;
	}

	bool RStackAllocator::PopAllocation()
	{
		if (mCurrentOffset == 0 || mLastAllocation.memoryOffset == 0)
		{
			RLOG_WARNING("Tried to pop a stack allocation while the allocator is empty.");
			return false;
		}

		// Move back the allocation offset.
		mCurrentOffset -= mLastAllocation.allocationSize + sizeof(FStackHeader);

		// Retrieve the stack header.
		FStackHeader* header = (FStackHeader*)((uint32_t*)mMemory + mCurrentOffset);

		// Set the previous allocation info.
		mLastAllocation.allocationSize = header->prevAllocationSize;
		mLastAllocation.memoryOffset = mCurrentOffset - header->prevAllocationSize;
		mLastAllocation.ptr = (uint32_t*)mMemory + mLastAllocation.memoryOffset;

		return true;
	}
}