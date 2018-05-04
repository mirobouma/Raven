#pragma once
#include <cinttypes>
#include <vector>
#include <memory>
#include "Core/Debug/LogManager.h"
#include "Core/Memory/MemoryUtils.h"
#include <algorithm>

#undef min
#undef max

namespace Raven
{

	// Implemented using the following article: http://bitsquid.blogspot.nl/2015/08/allocation-adventures-3-buddy-allocator.html

	static const uint32_t LEAF_SIZE = 64; // minimum allocation size.
	static const uint32_t MAX_LEVELS = 32; /*!< maximum amount of levels in the buddy allocator (~4gb * leaf size) */

	/*! Buddy allocation block. */
	struct FBuddyBlock {
		void* prev;
		void* next;
	};

	/*! \class RBuddyAllocator
	\brief Allocator which allocates memory using the Buddy allocator algorithm.
	Therefore, the size of the blocks are always a power of two, and you must guarantee that the initial size is a power of two.
	\sa RPoolAllocator
	\sa RFreeListAllocator
	\sa RLinearAllocator
	\sa RStackAllocator
	*/
	class RBuddyAllocator
	{
	public:
		/*! Buddy allocator.
		\param startSize Start size (will be round up to power of two).
		*/
		RBuddyAllocator(uint32_t startSize)
		{
			mInitialized = false;

			// Initialize free lists to null pointers.
			memset(&mFreeLists, 0, sizeof(void*)*MAX_LEVELS);

			// Calculate power-of-two size.
			mMaxSize = Memory::AlignUpToNextPowerOfTwo(std::max(startSize, LEAF_SIZE));

			// Calculate amount of levels for this buddy allocator.
			mAmountOfLevels = (uint32_t)(log2(mMaxSize) - log2(LEAF_SIZE)); // minus the amount of levels of the leaf size since it's the base.

			// Make sure that we don't exceed the maximum amount of levels.
			RASSERT(mAmountOfLevels < MAX_LEVELS);

			// Allocate memory for this buddy allocator.
			mMemory = malloc(mMaxSize);

			// allocate memory for the metadata flags.
			AllocateMetadata(mAmountOfLevels);
		}

		~RBuddyAllocator()
		{
			RASSERT(mMemory != nullptr);
			free(mMemory);
		}

		/*! Allocates memory from a memory block.
		\param amount Amount of objects to allocate.
		*/
		void* Allocate(uint32_t size)
		{
			// if the list of free blocks at level n is empty
			// allocate a block at level n - 1 (using this algorithm)
			// split the block into two blocks at level n
			// insert the two blocks into the list of free blocks for level n
			//remove the first block from the list at level n and return it


			// Calculate at which level we want to have this allocation.
			uint32_t levelOfAllocation = std::min((uint32_t)log2(Memory::AlignUpToNextPowerOfTwo((uint32_t)std::max(size + sizeof(FBuddyBlock), (size_t)LEAF_SIZE))), mAmountOfLevels);
			
			if (mFreeLists[levelOfAllocation] == nullptr)
			{
				// Allocate a block at level - 1.
				AllocateAndSplitBlock(levelOfAllocation - 1);
			}

			if (!mInitialized)
			{
				// We are allocating memory for metadata.
			}
			else
			{
				// We are allocating regular memory, therefore we also need to write metadata.
			}

			return nullptr;
		}

		/*! Free the allocations previously made for reusage */
		void Free(void* alloc)
		{

		}

		/*! Resets memory allocations for this buddy allocator (invalidates all memory allocated from this allocator.
		*/
		void Reset()
		{
			mInitialized = false;

			// Initialize free lists to null pointers.
			memset(&mFreeLists, 0, sizeof(void*) * MAX_LEVELS);

			// Reset any allocation metadata.
			memset(mMemory, 0, (1 << mAmountOfLevels) >> 3); // >> 3 to convert bits to bytes.
		}

	protected:

		/*! Allocate a block at a specified level. */
		void* AllocateAndSplitBlock(uint32_t level)
		{
			return 0;
		}

		/*! Allocates memory to store metadata flags. */
		void AllocateMetadata(uint32_t nLevels)
		{
			RASSERT(!mInitialized);

			uint32_t numAllocations = Memory::AlignUp((1 << nLevels) >> 3, LEAF_SIZE) / LEAF_SIZE;

			// Allocate memory for the blocks at the beginning of the buffer.
			for (uint32_t i = 0; i < numAllocations; ++i)
			{
				Allocate(LEAF_SIZE);
			}

			// initialize the metadata to null
			memset(mMemory, 0, (1 << nLevels) >> 3); // >> 3 to convert bits to bytes.
			mInitialized = true;
		}

		inline uint32_t GetSizeOfLevel(uint32_t level)
		{
			return mMaxSize / (1 << level);
		}

		inline uint32_t GetIndexInLevel(void* ptr, uint32_t level)
		{
			return (uint32_t)((char*)ptr - (char*)mMemory) / GetSizeOfLevel(level);
		}

		inline uint32_t GetMaxBlocksOfLevel(uint32_t level)
		{
			return (1 << level);
		}

		void* mMemory; /*!< Memory allocation for this buddy allocator. */
		void *mFreeLists[MAX_LEVELS]; /*!< Array of linked lists of free blocks. */
		uint32_t mMaxSize; /*!< Size of the object type for this pool allocator. */
		uint32_t mAmountOfLevels; /*!< Size of the object type for this pool allocator. */
		bool	 mInitialized; /*!< Boolean to check if we have initialized the meta data memory. */
	};

}