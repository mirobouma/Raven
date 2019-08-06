#pragma once
#include "renderer_interface/command_pool.h"
#include "vulkan_renderer/vulkan.h"

namespace Raven
{
	/*! \struct FVulkanCommandPool
	\brief Wrapper around vulkan command pool. */
	struct FVulkanCommandPool : public ICommandPool 
	{
		VkCommandPool mPool;
	};
}