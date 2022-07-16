#pragma once
#include "rendering/image.h"
#include "util/logging_macros.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    /*! \struct FVulkanImage
    \brief Wrapper object around a vulkan image object.
    */
    struct VulkanImage : IImage
    {
        virtual const ImageCreateDesc* get_desc() const
        {
            return &desc;
        }

        VkImage					image;
        VkImageView				imageView;
        VmaAllocation			allocation;
        ImageCreateDesc		    desc;
        VkImageCreateInfo		vkImageDesc;
        VkImageViewCreateInfo	vkImageViewDesc;
        ResourceState 			state;
        size_t					textureId;
    };
}
