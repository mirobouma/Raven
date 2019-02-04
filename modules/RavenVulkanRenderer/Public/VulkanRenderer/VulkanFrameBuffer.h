#pragma once

#include "VulkanRenderer/Vulkan.h"
#include <vector>

namespace Raven
{
	struct FVulkanRenderPass;
	struct FVulkanRenderTarget;

	/*! \struct FVulkanFrameBufferCreateDesc
	\brief Structure describing the parameters for frame buffer creation. */
	struct FVulkanFrameBufferCreateDesc
	{
		FVulkanRenderPass*          renderPass;
		uint32_t                    numAttachments;
		FVulkanRenderTarget**       ppAttachments;
		uint32_t                    width;
		uint32_t                    height;
		uint32_t                    layers;
	};

	/*! \struct FVulkanFrameBuffer
	\brief Wrapper object around a vulkan frame buffer.
	*/
	struct FVulkanFrameBuffer
	{
		VkFramebuffer					mFrameBuffer; /*! Vulkan frame buffer object. */
		FVulkanFrameBufferCreateDesc	mDesc; /*!< Frame buffer creation arguments. */
	};
}