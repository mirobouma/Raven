#pragma once
#include "core/api/rendering/command_pool.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    /*! \struct FVulkanCommandPool
    \brief Wrapper around vulkan command pool. */
    struct VulkanCommandPool : public ICommandPool
    {
        VkCommandPool pool;
    };
}
