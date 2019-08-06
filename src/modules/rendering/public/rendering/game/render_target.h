#pragma once
#include "renderer_interface/irenderinterface.h"

namespace Raven
{
	class RRenderTarget 
	{
		RRenderTarget(IRenderInterface* renderer, IRenderTarget* target)
		{
			mRenderer = renderer;
			mResource = target;
		}

		RRenderTarget(IRenderInterface* renderer, uint32_t width, uint32_t height, ETextureFormat format, uint32_t sampleCount = 1, uint32_t sampleQuality = 0, bool readBack = false)
		{
			mRenderer = renderer;

			FRenderTargetCreateDesc createDesc;
			createDesc.format = format;
			createDesc.width = width;
			createDesc.height = height;
			createDesc.memoryType = readBack ? EMemoryType::MEMORY_TYPE_GPU_TO_CPU : EMemoryType::MEMORY_TYPE_GPU;
			createDesc.nativeSwapchainImage = nullptr;
			createDesc.sampleCount = sampleCount;
			createDesc.sampleQuality = sampleQuality;
			createDesc.tiling = readBack ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;
			createDesc.usage = (
				format == TEXTURE_FORMAT_D16_UNORM ||
				format == TEXTURE_FORMAT_D24_UNORM_S8_UINT ||
				format == TEXTURE_FORMAT_D32_FLOAT || 
				format == TEXTURE_FORMAT_D32_FLOAT_S8_UINT) ?
				FRenderTargetUsage::RENDER_TARGET_USAGE_DEPTH_STENCIL_ATTACHMENT : FRenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT;

			mRenderer->createRenderTarget(&mResource, &createDesc);
		}

		void Destroy()
		{
			if (mResource != nullptr)
			{
				mRenderer->destroyRenderTarget(mResource);
				mResource = nullptr;
			}
		}

		~RRenderTarget()
		{
			Destroy();
		}

		const IRenderTarget* getResource()
		{
			return mResource;
		}

	protected:
		IRenderInterface* mRenderer;
		IRenderTarget* mResource;
	};
}