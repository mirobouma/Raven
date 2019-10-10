#pragma once
#include "core/api/rendering/semaphore.h"
#include "renderer-vulkan/vulkan.h"

/*! \struct FVulkanSemaphore
\brief Wrapper object around VkSemaphore.
*/
struct VulkanSemaphore : ISemaphore
{
    VkSemaphore semaphore;
};
