#pragma once
#include "Rendering/Interfaces/IRenderTarget.h"
#include "Core/Debug/LogManager.h"
#include "Vulkan.h"

namespace Raven
{
	struct FVulkanTexture;

	/*! \struct FVulkanRenderTarget
	\brief Wrapper object around a vulkan render target.
	*/
	struct FVulkanRenderTarget : IRenderTarget
	{
		FVulkanTexture*			mTexture;
		FRenderTargetCreateDesc	mDesc;

		virtual ITexture* GetTexture() const override
		{
			return (ITexture*)mTexture;
		}

		virtual const FRenderTargetCreateDesc* GetDesc() const override
		{
			return &mDesc;
		}
	};
}