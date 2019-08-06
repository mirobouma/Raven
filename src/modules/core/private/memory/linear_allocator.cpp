#include "core/memory/linear_allocator.h"
#include "core/memory/memory_utils.h"
#include "hal/info.h"
#include "core/debug/log_manager.h"

namespace Raven {
	RLinearAllocator::RLinearAllocator(uint32_t pageSize)
	{
		if (pageSize == UINT32_MAX || pageSize == 0)
		{
			// Use system defined page size.
			pageSize = RPlatformInfo::GetPageSize();
		}

		mPageSize = pageSize;
		mCurrentPage = nullptr;
	}

	RLinearAllocator::~RLinearAllocator()
	{
		mMemPages.clear();
	}

	void RLinearAllocator::Reset(bool freeBlocks)
	{
		// If we want to free all previously allocated blocks, clear the mMemBlocks vector. 
		// the unique ptr should automatically call our deconstructor which will free it's underlying memory.
		if (freeBlocks)
		{
			mMemPages.clear();
			mFreePages.clear();
		}
		else
		{
			// All memory blocks will be added to the free blocks vector so we can clear it.
			mFreePages.resize(mMemPages.size());

			// Reset memory pointers for the blocks.
			for (int i = 0; i < mMemPages.size(); i++)
			{
				mMemPages[i]->currentOffset = 0;
				mFreePages[i] = mMemPages[i].get();
			}
		}

		mCurrentPage = nullptr;
	}

	FLinearAllocation RLinearAllocator::Allocate(uint32_t size, uint32_t alignment)
	{
		const size_t AlignmentMask = alignment - 1;

		// Assert that it's a power of two and larger than the size of a pointer.
		RASSERT((alignment >= sizeof(void*)));
		RASSERT((AlignmentMask & alignment) == 0);

		uint32_t alignedSize = Memory::AlignUp(size, alignment);

		if (mCurrentPage == nullptr)
		{
			if (mFreePages.size() > 0)
			{
				mCurrentPage = mFreePages[mFreePages.size() - 1];
				mFreePages.pop_back();
			}
			else
			{
				mCurrentPage = CreateNewPage(alignedSize > mPageSize ? Memory::AlignUp(alignedSize, mPageSize) : mPageSize);
			}
		}

		uint32_t alignedOffset = Memory::IsAligned(mCurrentPage->currentOffset, alignment) ? mCurrentPage->currentOffset : Memory::AlignUp(mCurrentPage->currentOffset, alignment);
		
		if (alignedOffset + alignedSize > mCurrentPage->pageSize)
		{
			// Create a new page with mPageSize or a multiple of mPageSize if the allocation exceeds the page size.
			mCurrentPage = CreateNewPage(alignedSize > mPageSize ? Memory::AlignUp(alignedSize, mPageSize): mPageSize);
		}

		// Define allocation.
		FLinearAllocation alloc;
		alloc.allocationSize = alignedSize;
		alloc.page = mCurrentPage;
		alloc.pageOffset = alignedOffset;
		alloc.ptr = (uint32_t*)mCurrentPage->begin + alignedOffset;

		// Reserve this memory for the new allocation in the block.
		mCurrentPage->currentOffset = alignedOffset + alignedSize;

		return alloc;
	}

	FLinearAllocationPage* RLinearAllocator::CreateNewPage(uint32_t pageSize)
	{
		std::unique_ptr<FLinearAllocationPage> ptr = std::unique_ptr<FLinearAllocationPage>(new FLinearAllocationPage(pageSize));
		FLinearAllocationPage* page = ptr.get();

		// Add the memory page to the pool of pages.
		mMemPages.push_back(std::move(ptr));

		return page;
	}

	FLinearAllocationPage::FLinearAllocationPage(uint32_t blockSize)
	{
		currentOffset = 0;
		pageSize = blockSize;

		// Align by block size.
		begin = Memory::AlignedMalloc(pageSize, blockSize);
	}

	FLinearAllocationPage::~FLinearAllocationPage()
	{
		// Free memory for this linear allocation page.
		Memory::AlignedFree(begin);
	}
}