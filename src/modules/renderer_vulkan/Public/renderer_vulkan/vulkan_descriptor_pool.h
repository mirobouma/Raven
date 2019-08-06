#pragma once
#include "renderer_interface/shared_defines.h"
#include "vulkan_renderer/vulkan.h"

namespace Raven
{
	struct FVulkanDescriptorPoolCreateDesc
	{
		VkDescriptorPoolCreateFlags flags;
		uint32_t					numPoolSizes;
		VkDescriptorPoolSize*		pPoolSizes;
		uint32_t					maxDescriptorSets;
	};

	/*! \struct FVulkanDescriptorPool
	\brief Wrapper structure around VkDescriptorPool to keep track of descriptor allocations.*/
	struct FVulkanDescriptorPool
	{
		VkDescriptorPool mDescriptorPool; /*!< Descriptor pool object. */
		uint32_t		 mMaxDescriptorSets; /*!< Maximum amount of allocated descriptor sets. */
		uint32_t		 mNumDescriptorSets; /*!< Amount of allocated descriptor sets. */
		VkDescriptorPoolCreateFlags mFlags; /*!< Descriptor pool creation flags. Used check if we can free individual descriptors. */
	};
}