#pragma once
#include "renderer_interface/queue.h"
#include "renderer_vulkan/vulkan.h"

namespace Raven 
{
	struct FVulkanDevice;

	/*! \struct FVulkanQueue
	\brief Wrapper object around VkQueue.
	*/
	struct FVulkanQueue : IQueue
	{
		VkQueue mQueue; /*!< Vulkan queue object for this wrapper. */
		FVulkanDevice* mDevice; /*!< Device associated with this queue. */
		uint32_t mQueueFamilyIndex; /*! Queue family index on the physical device. */
		uint32_t mQueueIndex; /*! Queue index on the queue family. */
	};
}