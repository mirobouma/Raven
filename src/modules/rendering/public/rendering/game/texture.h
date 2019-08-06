#pragma once
#include "renderer_interface/irenderinterface.h"

namespace Raven
{
	class RTexture2D
	{
		RTexture2D(IRenderInterface* renderer, IImage* lowLevelTexture)
		{
			mRenderer = renderer;
			mResource = lowLevelTexture;
		}

		RTexture2D(IRenderInterface* renderer, uint32_t width, uint32_t height, ETextureFormat format, uint32_t sampleCount = 1, uint32_t sampleQuality = 0, uint32_t mipLevels = 1, bool writeable = false, bool readBack = false)
		{
			mRenderer = renderer;

			FImageCreateDesc createDesc;
			createDesc.format = format;
			createDesc.width = width;
			createDesc.height = height;
			createDesc.depth = 1;
			createDesc.arrayLayers = 0;
			createDesc.baseArrayLayer = 0;
			createDesc.baseMipLevel = 0;
			createDesc.mipLevels = mipLevels;
			createDesc.type = EImageType::IMAGE_TYPE_2D;
			createDesc.memoryType = readBack ? EMemoryType::MEMORY_TYPE_GPU_TO_CPU : EMemoryType::MEMORY_TYPE_GPU;
			createDesc.nativeSwapchainImage = nullptr;
			createDesc.sampleCount = sampleCount;
			createDesc.sampleQuality = sampleQuality;
			
			createDesc.tiling = readBack ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;
			createDesc.usage = 
				FImageUsageFlags::IMAGE_USAGE_SAMPLED | 
				FImageUsageFlags::IMAGE_USAGE_TRANSFER_DST | 
				(writeable ? FImageUsageFlags::IMAGE_USAGE_STORAGE : (FImageUsageFlags)0) | 
				(readBack ? FImageUsageFlags::IMAGE_USAGE_TRANSFER_SRC : (FImageUsageFlags)0);

			mRenderer->createImage(&mResource, &createDesc);
		}

		void Destroy()
		{
			if (mResource != nullptr)
			{
				mRenderer->destroyImage(mResource);
				mResource = nullptr;
			}
		}

		~RTexture2D()
		{
			Destroy();
		}

		const IImage* getResource()
		{
			return mResource;
		}

	protected:
		IRenderInterface* mRenderer;
		IImage* mResource;
	};

	class RTexture2DArray
	{
		RTexture2DArray(IRenderInterface* renderer, IImage* lowLevelTexture)
		{
			mRenderer = renderer;
			mResource = lowLevelTexture;
		}

		RTexture2DArray(IRenderInterface* renderer, uint32_t width, uint32_t height, uint32_t layers, ETextureFormat format, uint32_t sampleCount = 1, uint32_t sampleQuality = 0, uint32_t mipLevels = 1, bool writeable = false, bool readBack = false)
		{
			mRenderer = renderer;

			FImageCreateDesc createDesc;
			createDesc.format = format;
			createDesc.width = width;
			createDesc.height = height;
			createDesc.depth = 1;
			createDesc.arrayLayers = layers;
			createDesc.baseArrayLayer = 0;
			createDesc.baseMipLevel = 0;
			createDesc.mipLevels = mipLevels;
			createDesc.type = EImageType::IMAGE_TYPE_2D;
			createDesc.memoryType = readBack ? EMemoryType::MEMORY_TYPE_GPU_TO_CPU : EMemoryType::MEMORY_TYPE_GPU;
			createDesc.nativeSwapchainImage = nullptr;
			createDesc.sampleCount = sampleCount;
			createDesc.sampleQuality = sampleQuality;

			createDesc.tiling = readBack ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;
			createDesc.usage =
				FImageUsageFlags::IMAGE_USAGE_SAMPLED |
				FImageUsageFlags::IMAGE_USAGE_TRANSFER_DST |
				(writeable ? FImageUsageFlags::IMAGE_USAGE_STORAGE : (FImageUsageFlags)0) |
				(readBack ? FImageUsageFlags::IMAGE_USAGE_TRANSFER_SRC : (FImageUsageFlags)0);

			mRenderer->createImage(&mResource, &createDesc);
		}

		void Destroy()
		{
			if (mResource != nullptr)
			{
				mRenderer->destroyImage(mResource);
				mResource = nullptr;
			}
		}

		~RTexture2DArray()
		{
			Destroy();
		}

		const IImage* getResource()
		{
			return mResource;
		}

	protected:
		IRenderInterface * mRenderer;
		IImage* mResource;
	};

	class RTexture3D
	{
		RTexture3D(IRenderInterface* renderer, IImage* lowLevelTexture)
		{
			mRenderer = renderer;
			mResource = lowLevelTexture;
		}

		RTexture3D(IRenderInterface* renderer, uint32_t width, uint32_t height, uint32_t depth, ETextureFormat format, uint32_t sampleCount = 1, uint32_t sampleQuality = 0, uint32_t mipLevels = 1, bool writeable = false, bool readBack = false)
		{
			mRenderer = renderer;

			FImageCreateDesc createDesc;
			createDesc.format = format;
			createDesc.width = width;
			createDesc.height = height;
			createDesc.depth = depth;
			createDesc.arrayLayers = 0;
			createDesc.baseArrayLayer = 0;
			createDesc.baseMipLevel = 0;
			createDesc.mipLevels = mipLevels;
			createDesc.type = EImageType::IMAGE_TYPE_3D;
			createDesc.memoryType = readBack ? EMemoryType::MEMORY_TYPE_GPU_TO_CPU : EMemoryType::MEMORY_TYPE_GPU;
			createDesc.nativeSwapchainImage = nullptr;
			createDesc.sampleCount = sampleCount;
			createDesc.sampleQuality = sampleQuality;

			createDesc.tiling = readBack ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;
			createDesc.usage =
				FImageUsageFlags::IMAGE_USAGE_SAMPLED |
				FImageUsageFlags::IMAGE_USAGE_TRANSFER_DST |
				(writeable ? FImageUsageFlags::IMAGE_USAGE_STORAGE : (FImageUsageFlags)0) |
				(readBack ? FImageUsageFlags::IMAGE_USAGE_TRANSFER_SRC : (FImageUsageFlags)0);

			mRenderer->createImage(&mResource, &createDesc);
		}

		void Destroy()
		{
			if (mResource != nullptr)
			{
				mRenderer->destroyImage(mResource);
				mResource = nullptr;
			}
		}

		~RTexture3D()
		{
			Destroy();
		}

		const IImage* getResource()
		{
			return mResource;
		}

	protected:
		IRenderInterface* mRenderer;
		IImage* mResource;
	};
}