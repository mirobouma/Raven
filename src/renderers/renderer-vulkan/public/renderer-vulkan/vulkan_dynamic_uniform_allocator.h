#pragma once
#include "core/api/rendering/buffer.h"
#include "renderer-vulkan/vulkan.h"
#include "renderer-vulkan/vulkan_buffer.h"
#include "core/os/os.h"
#include "core/vector.h"
#include "core/memory.h"
#include <algorithm>
#include <cinttypes>
#undef max

/*! \struct FVulkanLinearAllocationPage
\brief Structure representing a linear allocation page.
*/
struct VulkanLinearAllocationPage
{
    VulkanLinearAllocationPage(IRenderingAPI* renderer, uint32_t blockSize)
    {
        renderer = renderer;
        currentOffset = 0;
        pageSize = blockSize;

        BufferCreateDesc desc = {};
        desc.memoryType = MEMORY_TYPE_CPU_TO_GPU;
        desc.size = pageSize;
        desc.usage = BUFFER_USAGE_UNIFORM_BUFFER | BUFFER_USAGE_TRANSFER_DST;

        renderer->create_buffer(&buffer, &desc);
    }

    ~VulkanLinearAllocationPage()
    {
        // Free memory for this linear allocation page.
        renderer->destroy_buffer(buffer);
    }

    IRenderingAPI* renderer; /*!< Reference to the renderer this buffer belongs to. */
    IBuffer* buffer; /*!< Buffer for this page.*/
    uint32_t        pageSize;  /*!< Size of this linear allocation page in bytes. */
    uint32_t        currentOffset;	/*!< Current allocation offset within this linear allocation page. */
};

/*! \struct FVulkanLinearAllocation
\brief Structure representing a single allocation for a linear allocator.
*/
struct VulkanLinearAllocation
{
    uint32_t size;
    IBuffer* buffer;
    uint32_t offset;
};

/*! \struct FVulkanDynamicUniformAllocator
\brief Linear allocator for dynamic uniform buffer allocations.
*/
struct VulkanDynamicUniformAllocator
{
    /*! Linear allocator.
    \param blockSize size of a page, if the page size is set to UINT32_MAX , it will use the system's page size (recommended). default is 2mb.
    */
    VulkanDynamicUniformAllocator(IRenderingAPI* pRenderer, uint32_t pageSize = 0x200000)
    {
        _renderer = pRenderer;

        if (pageSize == UINT32_MAX || pageSize == 0)
        {
            // Use system defined page size.
            pageSize = std::max(
                memory_align_up(static_cast<uint32_t>(OS::get().get_page_size()), 256)
                , static_cast<uint32_t>(0x100000)
            );
        }

        pageSize = pageSize;
        _currentPage = nullptr;
    }

    ~VulkanDynamicUniformAllocator()
    {
        _memPages.clear();
    }

    /*! Allocates memory from a memory block.
    \param size Size of the memory allocation.
    \param alignment Alignment of the memory allocation, 256 by default.
    */
    VulkanLinearAllocation allocate(IRenderingAPI* renderer, uint32_t size, VkDeviceSize alignment = 256)
    {
        const size_t alignmentMask = alignment - 1;

        // Assert that it's a power of two and larger than the size of a pointer.
        RASSERT((alignment >= sizeof(void*)));
        RASSERT((alignmentMask & alignment) == 0);

        uint32_t alignedSize = memory_align_up(size, alignment);

        if (_currentPage == nullptr)
        {
            if (_freePages.size() > 0)
            {
                _currentPage = _freePages[_freePages.size() - 1];
                _freePages.pop_back();
            }
            else
            {
                _currentPage = create_new_page(renderer, alignedSize > _pageSize ? memory_align_up(alignedSize, _pageSize) : _pageSize);
            }
        }

        uint32_t alignedOffset = memory_is_aligned(_currentPage->currentOffset, alignment)
            ? _currentPage->currentOffset
            : memory_align_up(_currentPage->currentOffset, alignment);

        if (alignedOffset + alignedSize > _currentPage->pageSize)
        {
            // Create a new page with mPageSize or a multiple of mPageSize if the allocation exceeds the page size.
            _currentPage = create_new_page(renderer, alignedSize > _pageSize ? memory_align_up(alignedSize, _pageSize) : _pageSize);
        }

        // Define allocation.
        VulkanLinearAllocation alloc;
        alloc.size = alignedSize;
        alloc.buffer = _currentPage->buffer;
        alloc.offset = alignedOffset;

        // Reserve this memory for the new allocation in the block.
        _currentPage->currentOffset = alignedOffset + alignedSize;
        return alloc;
    }

    /*! Resets memory allocations for this linear allocator.
    \param freeBlocks If freeBlocks is false, the previously allocated blocks won't be freed and can be reused by this linear allocator.
    */
    void reset(bool freeBlocks = true)
    {
        // If we want to free all previously allocated blocks, clear the mMemBlocks vector. 
        // the unique ptr should automatically call our deconstructor which will free it's underlying memory.
        if (freeBlocks)
        {
            _memPages.clear();
            _freePages.clear();
        }
        else
        {
            // All memory blocks will be added to the free blocks vector so we can clear it.
            _freePages.resize(_memPages.size());

            // Reset memory pointers for the blocks.
            for (int i = 0; i < _memPages.size(); i++)
            {
                _memPages[i]->currentOffset = 0;
                _freePages[i] = _memPages[i].get();
            }
        }

        _currentPage = nullptr;
    }

protected:

    /*! Creates a new linear allocation page.
    \param pageSize size of the page allocation.
    */
    VulkanLinearAllocationPage* create_new_page(IRenderingAPI* renderer, uint32_t pageSize)
    {
        std::unique_ptr<VulkanLinearAllocationPage> ptr = std::unique_ptr<VulkanLinearAllocationPage>(new VulkanLinearAllocationPage(renderer, pageSize));
        VulkanLinearAllocationPage* page = ptr.get();

        // Add the memory page to the pool of pages.
        _memPages.push_back(std::move(ptr));

        return page;
    }

    IRenderingAPI* _renderer; /*!< Reference to the renderer this allocator belongs to. */
    VulkanLinearAllocationPage* _currentPage; /*!< Current page for memory allocation. */
    Vector<std::unique_ptr<VulkanLinearAllocationPage>> _memPages; /*!< Vector of linear allocations. */
    Vector<VulkanLinearAllocationPage*>	_freePages; /*!< Vector of pages that still have room for allocations. */
    uint32_t _pageSize; /*!< Size each page should be. */
};
