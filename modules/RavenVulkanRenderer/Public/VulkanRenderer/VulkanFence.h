#pragma once
#include "RenderInterface/IFence.h"
#include "VulkanRenderer/Vulkan.h"

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