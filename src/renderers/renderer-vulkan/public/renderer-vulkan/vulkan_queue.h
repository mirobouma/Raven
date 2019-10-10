#pragma once
#include "core/api/rendering/queue.h"
#include "renderer-vulkan/vulkan.h"

struct VulkanDevice;

/*! \struct FVulkanQueue
\brief Wrapper object around VkQueue.
*/
struct VulkanQueue : IQueue
{
    VkQueue vkQueue; /*!< Vulkan queue object for this wrapper. */
    VulkanDevice* device; /*!< Device associated with this queue. */
    uint32_t queueFamilyIndex; /*! Queue family index on the physical device. */
    uint32_t queueIndex; /*! Queue index on the queue family. */
};
