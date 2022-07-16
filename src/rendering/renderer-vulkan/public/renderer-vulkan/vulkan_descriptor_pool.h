#pragma once
#include "rendering/defines.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    struct VulkanDescriptorPoolCreateDesc
    {
        VkDescriptorPoolCreateFlags flags;
        uint32_t					numPoolSizes;
        VkDescriptorPoolSize* pPoolSizes;
        uint32_t					maxDescriptorSets;
    };

    /*! \struct FVulkanDescriptorPool
    \brief Wrapper structure around VkDescriptorPool to keep track of descriptor allocations.*/
    struct VulkanDescriptorPool
    {
        VkDescriptorPool descriptorPool; /*!< Descriptor pool object. */
        uint32_t		 maxDescriptorSets; /*!< Maximum amount of allocated descriptor sets. */
        uint32_t		 numDescriptorSets; /*!< Amount of allocated descriptor sets. */
        VkDescriptorPoolCreateFlags flags; /*!< Descriptor pool creation flags. Used check if we can free individual descriptors. */
    };
}
