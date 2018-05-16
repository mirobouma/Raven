#pragma once
#include "Rendering/Interfaces/SharedDefines.h"
#include "Vulkan.h"
#include <map>

namespace Raven
{
	/*! \struct FVulkanDynamicDescriptorSetCache
	\brief Manager object keeping track of currently bound descriptors and created descriptor sets. */
	struct FVulkanDynamicDescriptorSetCache
	{
		std::map<size_t, VkDescriptorSet>	  mSetMap;
		FVulkanDescriptorPool*				  mDescriptorPool;
	};
}