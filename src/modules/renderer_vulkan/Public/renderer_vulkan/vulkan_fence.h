#pragma once
#include "renderer_interface/fence.h"
#include "renderer_vulkan/vulkan.h"

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