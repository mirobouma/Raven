#pragma once
#include "Rendering/Interfaces/ISemaphore.h"
#include "RenderingVulkan/Vulkan.h"

namespace Raven
{
	/*! \struct FVulkanSemaphore
	\brief Wrapper object around VkSemaphore.
	*/
	struct FVulkanSemaphore : ISemaphore
	{
		VkSemaphore mSemaphore;
	};
}