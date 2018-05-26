#pragma once
#include "Rendering/Interfaces/ITexture.h"
#include "Core/Debug/LogManager.h"
#include "Vulkan.h"

namespace Raven
{
	/*! \struct FVulkanTexture
	\brief Wrapper object around a vulkan image object.
	*/
	struct FVulkanTexture : ITexture
	{

		virtual const FTextureCreateDesc* GetDesc() const
		{
			return &mDesc;
		}

		VkImage					mImage;
		VkImageView				mImageView;
		VmaAllocation			mAllocation;
		FTextureCreateDesc		mDesc;
		VkImageCreateInfo		mVkImageDesc;
		VkImageViewCreateInfo	mVkImageViewDesc;
		FResourceState 			mState;
		size_t					mTextureId;
	};
}