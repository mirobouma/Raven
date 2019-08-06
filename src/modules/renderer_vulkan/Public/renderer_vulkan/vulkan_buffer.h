#pragma once
#include "renderer_interface/buffer.h"
#include "core/debug/log_manager.h"
#include "renderer_vulkan/vulkan.h"

namespace Raven
{
	/*! \struct FVulkanBuffer
	\brief Wrapper object around a vulkan buffer object.
	*/
	struct FVulkanBuffer : IBuffer
	{
		VkBuffer			mBuffer;
		VmaAllocation		mAllocation;
		FBufferCreateDesc	mDesc;
		FResourceState		mState;

		size_t getSize() override
		{
			return mDesc.size;
		}
	};
}