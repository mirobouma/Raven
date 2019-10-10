#pragma once
#include "core/api/rendering/fence.h"
#include "renderer-vulkan/vulkan.h"

/*! \struct FVulkanFence
\brief Wrapper object around VkFence.
*/
struct VulkanFence : IFence
{
    bool	submitted;
    VkFence fence;
};
