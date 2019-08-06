#pragma once
#include "renderer_interface/semaphore.h"
#include "renderer_interface/vulkan.h"

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