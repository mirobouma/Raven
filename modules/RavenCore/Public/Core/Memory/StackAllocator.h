#pragma once
#include <cinttypes>
#include <vector>
#include <memory>

namespace Raven
{
	/*! \struct FLinearAllocation
	\brief Structure representing a single allocation for a stack allocator.
	*/
	struct FStackAllocation
	{
		void* ptr;
		uint32_t allocationSize;
		uint32_t memoryOffset;
	};

	/*! \struct FStackHeader
	\brief Structure representing a header for a stack allocator.
	*/
	struct FStackHeader
	{
		uint32_t prevAllocationSize;
	};

	/*! \class RStackAllocator
	\brief Allocator which linearily allocates memory from a large chunk with headers to allow for stack-based pushing and popping of allocations.
	\sa RBuddyAllocator
	\sa RFreeListAllocator
	\sa RPoolAllocator
	\sa RSlabAllocator
	\sa RLinearAllocator
	*/
	class RStackAllocator
	{
	public:
		/*! Linear allocator.
		\param blockSize size of a page, if the page size is set to UINT32_MAX , it will use the system's page size (recommended). default is 2mb.
		*/
		RStackAllocator(uint32_t stackSize = 0x200000);
		~RStackAllocator();

		/*! Allocates memory from a memory block.
		\param size Size of the memory allocation.
		\param alignment Alignment of the memory allocation, 16 by default.
		\returns FStackAllocation on success, on failure, returns FStackAllocation with all members set to 0.
		*/
		FStackAllocation Allocate(uint32_t size, uint32_t alignment = 16);

		/*! Frees up the last allocated memory for reuse.
		*/
		bool PopAllocation();

		/*! Resets all memory allocations for this stack allocator.
		\param freeMemory If freeMemory is false, the previously stack memory won't be freed and can be reused by this stack allocator.
		*/
		void  Reset(bool freeMemory = false);

	protected:
		void* mMemory;
		uint32_t mStackSize;
		uint32_t mCurrentOffset;
		FStackAllocation mLastAllocation;
	};
}