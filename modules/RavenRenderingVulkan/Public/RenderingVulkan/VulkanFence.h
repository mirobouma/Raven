#pragma once
#include "Rendering/Interfaces/IFence.h"
#include "RenderingVulkan/Vulkan.h"

namespace Raven
{
	/*! \struct FVulkanFence
	\brief Wrapper object around VkFence.
	*/
	struct FVulkanFence : IFence
	{
		VkFence mFence;
	};
}