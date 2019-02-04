#pragma once
#include "RenderInterface/ICommandPool.h"
#include "VulkanRenderer/Vulkan.h"

namespace Raven
{
	/*! \struct FVulkanCommandPool
	\brief Wrapper around vulkan command pool. */
	struct FVulkanCommandPool : public ICommandPool 
	{
		VkCommandPool mPool;
	};
}