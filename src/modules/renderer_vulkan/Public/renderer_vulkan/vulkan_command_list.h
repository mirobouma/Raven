#pragma once
#include "renderer_interface/command_list.h"
#include "renderer_interface/shared_defines.h"
#include "renderer_vulkan/vulkan.h"
#include "renderer_vulkan/vulkan_caches.h"
#include "renderer_vulkan/vulkan_dynamic_uniform_allocator.h"
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