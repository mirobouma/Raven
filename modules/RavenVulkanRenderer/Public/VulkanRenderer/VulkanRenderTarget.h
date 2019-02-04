#pragma once
#include "RenderInterface/IRenderTarget.h"
#include "Core/Debug/LogManager.h"
#include "Vulkan.h"

namespace Raven
{
	struct FVulkanImage;

	/*! \struct FVulkanRenderTarget
	\brief Wrapper object around a vulkan render target.
	*/
	struct FVulkanRenderTarget : IRenderTarget
	{
		FVulkanImage*			mImage;
		FRenderTargetCreateDesc	mDesc;

		virtual IImage* GetImage() const override
		{
			return (IImage*)mImage;
		}

		virtual const FRenderTargetCreateDesc* GetDesc() const override
		{
			return &mDesc;
		}
	};
}