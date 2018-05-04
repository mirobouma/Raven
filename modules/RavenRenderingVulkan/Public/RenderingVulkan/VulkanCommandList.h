#pragma once
#include "Rendering/Interfaces/ICommandList.h"
#include "RenderingVulkan/Vulkan.h"

namespace Raven
{
	/*! \struct FVulkanCommandList
	\brief wrapper around vulkan command buffer. */
	struct FVulkanCommandList : public ICommandList
	{
		VkCommandBuffer mCmdBuffer;
	};
}