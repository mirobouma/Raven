#pragma once
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    struct VulkanRenderPass;
    struct VulkanRenderTarget;

    /*! \struct FVulkanFrameBufferCreateDesc
    \brief Structure describing the parameters for frame buffer creation. */
    struct VulkanFrameBufferCreateDesc
    {
        VulkanRenderPass* renderPass;
        uint32_t                    numAttachments;
        VulkanRenderTarget** ppAttachments;
        uint32_t                    width;
        uint32_t                    height;
        uint32_t                    layers;
    };

    /*! \struct FVulkanFrameBuffer
    \brief Wrapper object around a vulkan frame buffer.
    */
    struct VulkanFrameBuffer
    {
        VkFramebuffer				frameBuffer; /*! Vulkan frame buffer object. */
        VulkanFrameBufferCreateDesc	desc; /*!< Frame buffer creation arguments. */
    };
}
