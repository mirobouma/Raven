#pragma once
#include "renderer_interface/buffer.h"
#include "vulkan_renderer/vulkan_buffer.h"
#include "vulkan_renderer/vulkan.h"
#include <cinttypes>
#include <vector>
#include "hal/info.h"
#include "core/memory/memory_utils.h"
#include <algorithm>
#undef max

namespace Raven
{
	/*! \struct FVulkanLinearAllocationPage
	\brief Structure representing a linear allocation page.
	*/
	struct FVulkanLinearAllocationPage
	{
		FVulkanLinearAllocationPage(IRenderInterface* renderer, uint32_t blockSize)
		{
			mRenderer = renderer;
			mCurrentOffset = 0;
			mPageSize = blockSize;

			FBufferCreateDesc desc = {};
			desc.memoryType = MEMORY_TYPE_CPU_TO_GPU;
			desc.size = mPageSize;
			desc.usage = BUFFER_USAGE_UNIFORM_BUFFER | BUFFER_USAGE_TRANSFER_DST;

			mRenderer->createBuffer(&mBuffer, &desc);
		}

		~FVulkanLinearAllocationPage()
		{
			// Free memory for this linear allocation page.
			mRenderer->destroyBuffer(mBuffer);
		}
		
		IRenderInterface* mRenderer; /*!< Reference to the renderer this buffer belongs to. */
		IBuffer*   mBuffer; /*!< Buffer for this page.*/
		uint32_t   mPageSize;  /*!< Size of this linear allocation page in bytes. */
		uint32_t   mCurrentOffset;	/*!< Current allocation offset within this linear allocation page. */
	};

	/*! \struct FVulkanLinearAllocation
	\brief Structure representing a single allocation for a linear allocator.
	*/
	struct FVulkanLinearAllocation
	{
		uint32_t mSize;
		IBuffer* mBuffer;
		uint32_t mOffset;
	};

	/*! \struct FVulkanDynamicUniformAllocator
	\brief Linear allocator for dynamic uniform buffer allocations.
	*/
	struct FVulkanDynamicUniformAllocator
	{
		/*! Linear allocator.
		\param blockSize size of a page, if the page size is set to UINT32_MAX , it will use the system's page size (recommended). default is 2mb.
		*/
		FVulkanDynamicUniformAllocator(IRenderInterface* pRenderer, uint32_t pageSize = 0x200000)
		{
			mRenderer = pRenderer;

			if (pageSize == UINT32_MAX || pageSize == 0)
			{
				// Use system defined page size.
				pageSize = std::max(
					Memory::AlignUp(static_cast<uint32_t>(RPlatformInfo::GetPageSize()), 256)
					, static_cast<uint32_t>(0x100000)
				);
			}

			mPageSize = pageSize;
			mCurrentPage = nullptr;
		}

		~FVulkanDynamicUniformAllocator()
		{
			mMemPages.clear();
		}

		/*! Allocates memory from a memory block.
		\param size Size of the memory allocation.
		\param alignment Alignment of the memory allocation, 256 by default.
		*/
		FVulkanLinearAllocation Allocate(IRenderInterface* renderer, uint32_t size, uint32_t alignment = 256)
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
					mCurrentPage = CreateNewPage(renderer, alignedSize > mPageSize ? Memory::AlignUp(alignedSize, mPageSize) : mPageSize);
				}
			}

			uint32_t alignedOffset = Memory::IsAligned(mCurrentPage->mCurrentOffset, alignment) ? mCurrentPage->mCurrentOffset : Memory::AlignUp(mCurrentPage->mCurrentOffset, alignment);

			if (alignedOffset + alignedSize > mCurrentPage->mPageSize)
			{
				// Create a new page with mPageSize or a multiple of mPageSize if the allocation exceeds the page size.
				mCurrentPage = CreateNewPage(renderer, alignedSize > mPageSize ? Memory::AlignUp(alignedSize, mPageSize) : mPageSize);
			}

			// Define allocation.
			FVulkanLinearAllocation alloc;
			alloc.mSize = alignedSize;
			alloc.mBuffer = mCurrentPage->mBuffer;
			alloc.mOffset = alignedOffset;

			// Reserve this memory for the new allocation in the block.
			mCurrentPage->mCurrentOffset = alignedOffset + alignedSize;

			return alloc;
		}

		/*! Resets memory allocations for this linear allocator.
		\param freeBlocks If freeBlocks is false, the previously allocated blocks won't be freed and can be reused by this linear allocator.
		*/
		void  Reset(bool freeBlocks = true)
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
					mMemPages[i]->mCurrentOffset = 0;
					mFreePages[i] = mMemPages[i].get();
				}
			}

			mCurrentPage = nullptr;
		}

	protected:

		/*! Creates a new linear allocation page.
		\param pageSize size of the page allocation.
		*/
		FVulkanLinearAllocationPage* CreateNewPage(IRenderInterface* renderer, uint32_t pageSize)
		{
			std::unique_ptr<FVulkanLinearAllocationPage> ptr = std::unique_ptr<FVulkanLinearAllocationPage>(new FVulkanLinearAllocationPage(renderer, pageSize));
			FVulkanLinearAllocationPage* page = ptr.get();

			// Add the memory page to the pool of pages.
			mMemPages.push_back(std::move(ptr));

			return page;
		}

		IRenderInterface*													mRenderer; /*!< Reference to the renderer this allocator belongs to. */
		FVulkanLinearAllocationPage*								mCurrentPage; /*!< Current page for memory allocation. */
		std::vector<std::unique_ptr<FVulkanLinearAllocationPage>>	mMemPages; /*!< Vector of linear allocations. */
		std::vector<FVulkanLinearAllocationPage*>					mFreePages; /*!< Vector of pages that still have room for allocations. */
		uint32_t													mPageSize; /*!< Size each page should be. */
	};
}