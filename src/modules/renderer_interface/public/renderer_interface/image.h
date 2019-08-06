#pragma once
#include "renderer_interface/shared_defines.h"

namespace Raven 
{
	typedef enum EImageType
	{
		IMAGE_TYPE_1D,
		IMAGE_TYPE_2D,
		IMAGE_TYPE_3D,
		IMAGE_TYPE_CUBE,
		IMAGE_TYPE
	} EImageType;

	typedef enum EImageTiling
	{
		IMAGE_TILING_OPTIMAL,
		IMAGE_TILING_LINEAR,
		MAX_IMAGE_TILING
	} EImageTiling;

	typedef enum FImageUsageFlags
	{
		IMAGE_USAGE_NONE = 0,
		IMAGE_USAGE_TRANSFER_SRC = 1 << 0,
		IMAGE_USAGE_TRANSFER_DST = 1 << 1,
		IMAGE_USAGE_SAMPLED = 1 << 2,
		IMAGE_USAGE_STORAGE = 1 << 3,
		IMAGE_USAGE_COLOR_ATTACHMENT = 1 << 4,
		IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 1 << 5,
		IMAGE_USAGE_TRANSIENT_ATTACHMENT = 1 << 6,
		IMAGE_USAGE_INPUT_ATTACHMENT = 1 << 7,
		MAX_IMAGE_TARGET_USAGE = 8
	} FImageUsageFlags;
	DECL_ENUM_AS_FLAGS(FImageUsageFlags)


	/*! \struct FImageCreateDesc
	\brief Structure describing the parameters for image creation. */
	struct FImageCreateDesc
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

		EImageType				type;
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
		FImageUsageFlags		usage;
		EImageTiling			tiling;
		EMemoryType				memoryType;
		void*					pData;
		size_t					dataSize;
		void*					nativeSwapchainImage;
	};

	/*! \struct IImage 
	\brief Interface for image resource. */
	struct IImage {
		virtual const FImageCreateDesc* GetDesc() const = 0;
	};
}