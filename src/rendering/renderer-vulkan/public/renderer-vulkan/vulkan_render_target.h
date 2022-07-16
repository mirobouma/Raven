#pragma once
#include "rendering/render_target.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    struct VulkanImage;

    /*! \struct FVulkanRenderTarget
    \brief Wrapper object around a vulkan render target.
    */
    struct VulkanRenderTarget : IRenderTarget
    {
        VulkanImage* image;
        RenderTargetCreateDesc	desc;

        virtual IImage* get_image() const override
        {
            return (IImage*)image;
        }

        virtual const RenderTargetCreateDesc* get_desc() const override
        {
            return &desc;
        }
    };
}
