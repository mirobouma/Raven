#pragma once
#include "rendering/buffer.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    /*! \struct FVulkanBuffer
    \brief Wrapper object around a vulkan buffer object.
    */
    struct VulkanBuffer : IBuffer
    {
        VkBuffer			buffer;
        VmaAllocation		allocation;
        BufferCreateDesc	desc;
        ResourceState		state;

        size_t get_size() override
        {
            return desc.size;
        }
    };
}
