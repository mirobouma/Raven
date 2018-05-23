#pragma once
#include "Rendering/Interfaces/IRenderer.h"

namespace Raven
{
	class RTexture2D
	{
		RTexture2D(IRenderer* renderer, ITexture* lowLevelTexture)
		{
			mRenderer = renderer;
			mResource = lowLevelTexture;
		}

		RTexture2D(IRenderer* renderer, uint32_t width, uint32_t height, ETextureFormat format, uint32_t sampleCount = 1, uint32_t sampleQuality = 0, uint32_t mipLevels = 1, bool writeable = false, bool readBack = false)
		{
			mRenderer = renderer;

			FTextureCreateDesc createDesc;
			createDesc.format = format;
			createDesc.width = width;
			createDesc.height = height;
			createDesc.depth = 1;
			createDesc.arrayLayers = 0;
			createDesc.baseArrayLayer = 0;
			createDesc.baseMipLevel = 0;
			createDesc.mipLevels = mipLevels;
			createDesc.type = ETextureType::TEXTURE_TYPE_2D;
			createDesc.memoryType = readBack ? EMemoryType::MEMORY_TYPE_GPU_TO_CPU : EMemoryType::MEMORY_TYPE_GPU;
			createDesc.nativeSwapchainImage = nullptr;
			createDesc.sampleCount = sampleCount;
			createDesc.sampleQuality = sampleQuality;
			
			createDesc.tiling = readBack ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;
			createDesc.usage = 
				FTextureUsageFlags::TEXTURE_USAGE_SAMPLED | 
				FTextureUsageFlags::TEXTURE_USAGE_TRANSFER_DST | 
				(writeable ? FTextureUsageFlags::TEXTURE_USAGE_STORAGE : (FTextureUsageFlags)0) | 
				(readBack ? FTextureUsageFlags::TEXTURE_USAGE_TRANSFER_SRC : (FTextureUsageFlags)0);

			mRenderer->createTexture(&mResource, &createDesc);
		}

		void Destroy()
		{
			if (mResource != nullptr)
			{
				mRenderer->destroyTexture(mResource);
				mResource = nullptr;
			}
		}

		~RTexture2D()
		{
			Destroy();
		}

		const ITexture* getResource()
		{
			return mResource;
		}

	protected:
		IRenderer* mRenderer;
		ITexture* mResource;
	};

	class RTexture2DArray
	{
		RTexture2DArray(IRenderer* renderer, ITexture* lowLevelTexture)
		{
			mRenderer = renderer;
			mResource = lowLevelTexture;
		}

		RTexture2DArray(IRenderer* renderer, uint32_t width, uint32_t height, uint32_t layers, ETextureFormat format, uint32_t sampleCount = 1, uint32_t sampleQuality = 0, uint32_t mipLevels = 1, bool writeable = false, bool readBack = false)
		{
			mRenderer = renderer;

			FTextureCreateDesc createDesc;
			createDesc.format = format;
			createDesc.width = width;
			createDesc.height = height;
			createDesc.depth = 1;
			createDesc.arrayLayers = layers;
			createDesc.baseArrayLayer = 0;
			createDesc.baseMipLevel = 0;
			createDesc.mipLevels = mipLevels;
			createDesc.type = ETextureType::TEXTURE_TYPE_2D;
			createDesc.memoryType = readBack ? EMemoryType::MEMORY_TYPE_GPU_TO_CPU : EMemoryType::MEMORY_TYPE_GPU;
			createDesc.nativeSwapchainImage = nullptr;
			createDesc.sampleCount = sampleCount;
			createDesc.sampleQuality = sampleQuality;

			createDesc.tiling = readBack ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;
			createDesc.usage =
				FTextureUsageFlags::TEXTURE_USAGE_SAMPLED |
				FTextureUsageFlags::TEXTURE_USAGE_TRANSFER_DST |
				(writeable ? FTextureUsageFlags::TEXTURE_USAGE_STORAGE : (FTextureUsageFlags)0) |
				(readBack ? FTextureUsageFlags::TEXTURE_USAGE_TRANSFER_SRC : (FTextureUsageFlags)0);

			mRenderer->createTexture(&mResource, &createDesc);
		}

		void Destroy()
		{
			if (mResource != nullptr)
			{
				mRenderer->destroyTexture(mResource);
				mResource = nullptr;
			}
		}

		~RTexture2DArray()
		{
			Destroy();
		}

		const ITexture* getResource()
		{
			return mResource;
		}

	protected:
		IRenderer * mRenderer;
		ITexture* mResource;
	};

	class RTexture3D
	{
		RTexture3D(IRenderer* renderer, ITexture* lowLevelTexture)
		{
			mRenderer = renderer;
			mResource = lowLevelTexture;
		}

		RTexture3D(IRenderer* renderer, uint32_t width, uint32_t height, uint32_t depth, ETextureFormat format, uint32_t sampleCount = 1, uint32_t sampleQuality = 0, uint32_t mipLevels = 1, bool writeable = false, bool readBack = false)
		{
			mRenderer = renderer;

			FTextureCreateDesc createDesc;
			createDesc.format = format;
			createDesc.width = width;
			createDesc.height = height;
			createDesc.depth = depth;
			createDesc.arrayLayers = 0;
			createDesc.baseArrayLayer = 0;
			createDesc.baseMipLevel = 0;
			createDesc.mipLevels = mipLevels;
			createDesc.type = ETextureType::TEXTURE_TYPE_3D;
			createDesc.memoryType = readBack ? EMemoryType::MEMORY_TYPE_GPU_TO_CPU : EMemoryType::MEMORY_TYPE_GPU;
			createDesc.nativeSwapchainImage = nullptr;
			createDesc.sampleCount = sampleCount;
			createDesc.sampleQuality = sampleQuality;

			createDesc.tiling = readBack ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;
			createDesc.usage =
				FTextureUsageFlags::TEXTURE_USAGE_SAMPLED |
				FTextureUsageFlags::TEXTURE_USAGE_TRANSFER_DST |
				(writeable ? FTextureUsageFlags::TEXTURE_USAGE_STORAGE : (FTextureUsageFlags)0) |
				(readBack ? FTextureUsageFlags::TEXTURE_USAGE_TRANSFER_SRC : (FTextureUsageFlags)0);

			mRenderer->createTexture(&mResource, &createDesc);
		}

		void Destroy()
		{
			if (mResource != nullptr)
			{
				mRenderer->destroyTexture(mResource);
				mResource = nullptr;
			}
		}

		~RTexture3D()
		{
			Destroy();
		}

		const ITexture* getResource()
		{
			return mResource;
		}

	protected:
		IRenderer* mRenderer;
		ITexture* mResource;
	};
}