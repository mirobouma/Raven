#pragma once
#include "renderer_interface/image.h"
#include "core/debug/log_manager.h"
#include "renderer_vulkan/vulkan.h"

namespace Raven
{
	/*! \struct FVulkanImage
	\brief Wrapper object around a vulkan image object.
	*/
	struct FVulkanImage : IImage
	{

		virtual const FImageCreateDesc* GetDesc() const
		{
			return &mDesc;
		}

		VkImage					mImage;
		VkImageView				mImageView;
		VmaAllocation			mAllocation;
		FImageCreateDesc		mDesc;
		VkImageCreateInfo		mVkImageDesc;
		VkImageViewCreateInfo	mVkImageViewDesc;
		FResourceState 			mState;
		size_t					mTextureId;
	};
}