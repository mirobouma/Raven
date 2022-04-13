#pragma once
#include "core/api/rendering/command_list.h"
#include "core/api/rendering/defines.h"
#include "renderer-vulkan/vulkan.h"
#include "renderer-vulkan/vulkan_caches.h"
#include "renderer-vulkan/vulkan_dynamic_uniform_allocator.h"
#include <map>
#include <tuple>

namespace Raven
{
    struct VulkanPipeline;
    struct VulkanImage;
    struct VulkanBuffer;
    struct VulkanRenderPass;

    /*! \struct FVulkanCommandList
    \brief wrapper around vulkan command buffer. */
    struct VulkanCommandList : public ICommandList
    {
        VkCommandBuffer	cmdBuffer;
        VulkanPipeline* boundPipeline;
        VulkanRenderPass* currentRenderPass;
        VulkanDynamicDescriptorSetCache* descriptorSetCache;
        VulkanDynamicUniformAllocator* dynamicUniformAllocator;
        std::map<VulkanImage*, std::tuple<ResourceState, ResourceState>> pendingImageBarriers;
        std::map<VulkanBuffer*, std::tuple<ResourceState, ResourceState>> pendingBufferBarriers;
    };
}
