#pragma once

#include "RenderingVulkan/Vulkan.h"
#include <vector>

namespace Raven
{
	struct FVulkanRenderTarget;

	/*! \struct FVulkanRenderPassCreateDesc
	\brief Structure describing the parameters for render pass creation. */
	struct FVulkanRenderPassCreateDesc
	{
		uint32_t		numColorAttachments;
		ETextureFormat	colorAttachmentFormats[8];
		ETextureFormat	depthStencilAttachmentFormat;
		FLoadStoreOps*	pLoadStoreOps;
		uint32_t		sampleCount;
		uint32_t		sampleQuality;
	};

	/*! \struct FVulkanRenderPass
	\brief Wrapper object around a vulkan render pass.
	*/
	struct FVulkanRenderPass
	{
		VkRenderPass				mPass; /*!< Vulkan Render pass pointer. */
		FVulkanRenderPassCreateDesc mDesc; /*!< Render pass creation arguments. */
	};
}