#pragma once
#include "SharedDefines.h"

namespace Raven 
{
	typedef enum ETextureType
	{
		TEXTURE_TYPE_1D,
		TEXTURE_TYPE_2D,
		TEXTURE_TYPE_3D,
		TEXTURE_TYPE_CUBE,
		MAX_TEXTURE_TYPE
	} ETextureType;

	typedef enum EImageTiling
	{
		IMAGE_TILING_OPTIMAL,
		IMAGE_TILING_LINEAR,
		MAX_IMAGE_TILING
	} EImageTiling;

	typedef enum FTextureUsageFlags
	{
		TEXTURE_USAGE_NONE = 0,
		TEXTURE_USAGE_TRANSFER_SRC = 1 << 0,
		TEXTURE_USAGE_TRANSFER_DST = 1 << 1,
		TEXTURE_USAGE_SAMPLED = 1 << 2,
		TEXTURE_USAGE_STORAGE = 1 << 3,
		TEXTURE_USAGE_COLOR_ATTACHMENT = 1 << 4,
		TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT = 1 << 5,
		TEXTURE_USAGE_TRANSIENT_ATTACHMENT = 1 << 6,
		TEXTURE_USAGE_INPUT_ATTACHMENT = 1 << 7,
		MAX_TEXTURE_TARGET_USAGE = 8
	} FTextureUsageFlags;
	DECL_ENUM_AS_FLAGS(FTextureUsageFlags)


	/*! \struct FTextureCreateDesc
	\brief Structure describing the parameters for texture creation. */
	struct FTextureCreateDesc
	{
		//VkImageCreateFlags       flags;
		//VkImageType              imageType;
		//VkFormat                 format;
		//VkExtent3D               extent;
		//uint32_t                 mipLevels;
		//uint32_t                 arrayLayers;
		//VkSampleCountFlagBits    samples;
		//VkImageTiling            tiling;
		//VkImageUsageFlags        usage;
		//VkSharingMode            sharingMode;
		//uint32_t                 queueFamilyIndexCount;
		//const uint32_t*          pQueueFamilyIndices;
		//VkImageLayout            initialLayout;

		ETextureType			type;
		uint32_t				width;
		uint32_t				height;
		uint32_t				depth;
		uint32_t				arrayLayers;
		uint32_t				baseArrayLayer;
		uint32_t				mipLevels;
		uint32_t				baseMipLevel;
		uint32_t				sampleCount;
		uint32_t				sampleQuality;
		ETextureFormat			format;
		FTextureUsageFlags		usage;
		EImageTiling			tiling;
		EMemoryType				memoryType;
		void*					pData;
		size_t					dataSize;
		void*					nativeSwapchainImage;
	};

	/*! \struct ITexture 
	\brief Interface for texture resource. */
	struct ITexture {
		virtual const FTextureCreateDesc* GetDesc() const = 0;
	};
}