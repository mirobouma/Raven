#pragma once
#include "RenderInterface/ISemaphore.h"
#include "VulkanRenderer/Vulkan.h"

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