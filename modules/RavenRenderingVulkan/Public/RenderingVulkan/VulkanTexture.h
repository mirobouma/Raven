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
		VkImage				mImage;
		VkImageView			mImageView;
		VkDeviceMemory		mMemory;
		FTextureCreateDesc	mDesc;
	};
}