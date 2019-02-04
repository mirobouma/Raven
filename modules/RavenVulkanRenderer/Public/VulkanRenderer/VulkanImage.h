#pragma once
#include "RenderInterface/IImage.h"
#include "Core/Debug/LogManager.h"
#include "Vulkan.h"

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