#pragma once
#include "renderer-vulkan/vulkan.h"
#include "core/vector.h"

namespace Raven
{
    struct VulkanRenderTarget;

    /*! \struct FVulkanRenderPassCreateDesc
    \brief Structure describing the parameters for render pass creation. */
    struct VulkanRenderPassCreateDesc
    {
        uint32_t		numColorAttachments;
        TextureFormat	colorAttachmentFormats[8];
        TextureFormat	depthStencilAttachmentFormat;
        LoadStoreOps* pLoadStoreOps;
        uint32_t		sampleCount;
        uint32_t		sampleQuality;
    };

    /*! \struct FVulkanRenderPass
    \brief Wrapper object around a vulkan render pass.
    */
    struct VulkanRenderPass
    {
        VkRenderPass				pass; /*!< Vulkan Render pass pointer. */
        VulkanRenderPassCreateDesc desc; /*!< Render pass creation arguments. */
    };
}
