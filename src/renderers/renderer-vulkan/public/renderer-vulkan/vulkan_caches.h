#pragma once
#include "core/api/rendering/defines.h"
#include "core/vector.h"
#include "renderer-vulkan/vulkan_defines.h"
#include "renderer-vulkan/vulkan.h"
#include "renderer-vulkan/vulkan_frame_buffer.h"
#include "renderer-vulkan/vulkan_render_pass.h"
#include <map>

namespace Raven
{
    /*! \struct FVulkanDescriptorSetCache
    \brief Cache of descriptor sets. */
    struct VulkanDescriptorSetCache
    {
        VulkanDescriptorPool* descriptorPool;
        std::map<size_t, Vector<VkDescriptorSet>>	setMap[gMaxFramesAtOnce];
    };

    /*! \struct FVulkanDynamicDescriptorSetCache
    \brief Manager object keeping track of currently bound descriptors and created descriptor sets. */
    struct VulkanDynamicDescriptorSetCache
    {
        std::map<size_t, VkDescriptorSet> hashToSet;
        VulkanDescriptorPool* descriptorPool;
    };

    /*! \struct FVulkanFramebufferCache
    \brief Cache of frame buffer objects. */
    struct VulkanFramebufferCache
    {
        std::map<size_t, VulkanFrameBuffer*> hashToBuffer;
    };

    /*! \struct FVulkanRenderPassCache
    \brief Cache of render passes. */
    struct VulkanRenderPassCache
    {
        std::map<size_t, VulkanRenderPass*> hashToRenderPass;
    };
}
