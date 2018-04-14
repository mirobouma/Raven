#pragma once
#include <cinttypes>
#include <vector>
#include <memory>

namespace Raven
{

	/*! \struct FLinearAllocationPage
	\brief Structure representing a linear allocation page.
	*/
	struct FLinearAllocationPage
	{
		FLinearAllocationPage(uint32_t blockSize);
		~FLinearAllocationPage();

		void* begin; /*!< Begin pointer for this linear allocation page. */
		uint32_t pageSize;  /*!< Size of this linear allocation page in bytes. */
		uint32_t currentOffset;	/*!< Current allocation offset within this linear allocation page. */
	};

	/*! \struct FLinearAllocation
	\brief Structure representing a single allocation for a linear allocator.
	*/
	struct FLinearAllocation
	{
		void* ptr;
		uint32_t allocationSize;
		FLinearAllocationPage* page;
		uint32_t pageOffset;
	};

	/*! \class RLinearAllocator
	\brief Allocator which linearily allocates memory from preallocated chunks.
	\sa RBuddyAllocator
	\sa RFreeListAllocator
	\sa RPoolAllocator
	\sa RStackAllocator
	*/
	class RLinearAllocator
	{
	public:
		/*! Linear allocator.
		\param blockSize size of a page, if the page size is set to UINT32_MAX , it will use the system's page size (recommended). default is 2mb.
		*/
		RLinearAllocator(uint32_t pageSize = 0x200000);
		~RLinearAllocator();

		/*! Allocates memory from a memory block.
		\param size Size of the memory allocation.
		\param alignment Alignment of the memory allocation, 8 by default.
		*/
		FLinearAllocation Allocate(uint32_t size, uint32_t alignment = 16);

		/*! Resets memory allocations for this linear allocator.
		\param freeBlocks If freeBlocks is false, the previously allocated blocks won't be freed and can be reused by this linear allocator. 
		*/
		void  Reset(bool freeBlocks = true);

	protected:

		/*! Creates a new linear allocation page.
		\param pageSize size of the page allocation.
		*/
		FLinearAllocationPage* CreateNewPage(uint32_t pageSize);

		FLinearAllocationPage* mCurrentPage; /*!< Current page for memory allocation. */
		std::vector<std::unique_ptr<FLinearAllocationPage>> mMemPages;
		std::vector<FLinearAllocationPage*> mFreePages;
		uint32_t mPageSize;
	};

}