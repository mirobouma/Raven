#pragma once
#include "Rendering/Interfaces/ICommandList.h"
#include "Rendering/Interfaces/SharedDefines.h"
#include "RenderingVulkan/Vulkan.h"
#include "RenderingVulkan/VulkanDescriptorSetCache.h"
#include <map>
#include <tuple>

namespace Raven
{
	struct FVulkanPipeline;
	struct FVulkanTexture;
	struct FVulkanBuffer;
	struct FVulkanRenderPass;

	/*! \struct FVulkanCommandList
	\brief wrapper around vulkan command buffer. */
	struct FVulkanCommandList : public ICommandList
	{
		VkCommandBuffer															mCmdBuffer;
		FVulkanPipeline*														mBoundPipeline;
		FVulkanRenderPass*														mCurrentRenderPass;
		std::map<FVulkanTexture*, std::tuple<FResourceState, FResourceState>>	mPendingImageBarriers;
		std::map<FVulkanBuffer*, std::tuple<FResourceState, FResourceState>>	mPendingBufferBarriers;
		FVulkanDynamicDescriptorSetCache*										mDescriptorSetCache;
	};
}