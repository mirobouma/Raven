#pragma once
#include "renderer_interface/command_pool.h"
#include "renderer_vulkan/vulkan.h"

namespace Raven
{
	/*! \struct FVulkanCommandPool
	\brief Wrapper around vulkan command pool. */
	struct FVulkanCommandPool : public ICommandPool 
	{
		VkCommandPool mPool;
	};
}