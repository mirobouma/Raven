#pragma once
#include "RenderInterface/SharedDefines.h"
#include "VulkanRenderer/VulkanFrameBuffer.h"
#include "VulkanRenderer/VulkanRenderPass.h"
#include "Vulkan.h"
#include <map>

namespace Raven
{
	/*! \struct FVulkanDescriptorSetCache
	\brief Cache of descriptor sets. */
	struct FVulkanDescriptorSetCache
	{
		std::map<size_t, std::vector<VkDescriptorSet>>	mSetMap[MAX_FRAMES_AT_ONCE];
		FVulkanDescriptorPool*							mDescriptorPool;
	};

	/*! \struct FVulkanDynamicDescriptorSetCache
	\brief Manager object keeping track of currently bound descriptors and created descriptor sets. */
	struct FVulkanDynamicDescriptorSetCache
	{
		std::map<size_t, VkDescriptorSet>	  mSetMap;
		FVulkanDescriptorPool*				  mDescriptorPool;
	};

	/*! \struct FVulkanFramebufferCache
	\brief Cache of frame buffer objects. */
	struct FVulkanFramebufferCache
	{
		std::map<size_t, FVulkanFrameBuffer*> mHashMap;
	};

	/*! \struct FVulkanRenderPassCache
	\brief Cache of render passes. */
	struct FVulkanRenderPassCache
	{
		std::map<size_t, FVulkanRenderPass*> mHashMap;
	};
}