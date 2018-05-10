#pragma once
#include "Rendering/Interfaces/SharedDefines.h"
#include "Vulkan.h"
#include <map>

namespace Raven
{
	/*! \struct FVulkanDescriptorSetCache
	\brief Manager object keeping track of currently bound descriptors and created descriptor sets. */
	struct FVulkanDescriptorSetCache
	{
		std::map<size_t, VkDescriptorSet> mDescriptorSetCache;
	};
}