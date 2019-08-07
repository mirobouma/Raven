#pragma once
#include "renderer_interface/semaphore.h"
#include "renderer_vulkan/vulkan.h"

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