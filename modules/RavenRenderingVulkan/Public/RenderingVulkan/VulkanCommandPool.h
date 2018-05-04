#pragma once
#include "Rendering/Interfaces/ICommandPool.h"
#include "RenderingVulkan/Vulkan.h"

namespace Raven
{
	/*! \struct FVulkanCommandPool
	\brief Wrapper around vulkan command pool. */
	struct FVulkanCommandPool : public ICommandPool 
	{
		VkCommandPool mPool;
	};
}