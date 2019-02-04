#pragma once
#include "RenderInterface/ICommandList.h"
#include "RenderInterface/SharedDefines.h"
#include "VulkanRenderer/Vulkan.h"
#include "VulkanRenderer/VulkanCaches.h"
#include "VulkanRenderer/VulkanDynamicUniformAllocator.h"
#include <map>
#include <tuple>

namespace Raven
{
	struct FVulkanPipeline;
	struct FVulkanImage;
	struct FVulkanBuffer;
	struct FVulkanRenderPass;

	/*! \struct FVulkanCommandList
	\brief wrapper around vulkan command buffer. */
	struct FVulkanCommandList : public ICommandList
	{
		VkCommandBuffer															mCmdBuffer;
		FVulkanPipeline*														mBoundPipeline;
		FVulkanRenderPass*														mCurrentRenderPass;
		std::map<FVulkanImage*, std::tuple<FResourceState, FResourceState>>		mPendingImageBarriers;
		std::map<FVulkanBuffer*, std::tuple<FResourceState, FResourceState>>	mPendingBufferBarriers;
		FVulkanDynamicDescriptorSetCache*										mDescriptorSetCache;
		FVulkanDynamicUniformAllocator*											mDynamicUniformAllocator;
	};
}