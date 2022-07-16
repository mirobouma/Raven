#pragma once
#include "rendering/fence.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    /*! \struct FVulkanFence
    \brief Wrapper object around VkFence.
    */
    struct VulkanFence : IFence
    {
        bool	submitted;
        VkFence fence;
    };
}