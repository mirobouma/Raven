#pragma once
#include "renderer_interface/fence.h"
#include "vulkan_renderer/vulkan.h"

namespace Raven
{
	/*! \struct FVulkanFence
	\brief Wrapper object around VkFence.
	*/
	struct FVulkanFence : IFence
	{
		bool	mSubmitted;
		VkFence mFence;
	};
}