#pragma once
#include "renderer_interface/render_target.h"
#include "core/debug/log_manager.h"
#include "renderer_vulkan/vulkan.h"

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