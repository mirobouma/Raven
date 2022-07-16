#pragma once
#include "rendering/defines.h"
#include "util/enum_as_flags.h"

namespace Raven
{
    typedef enum ImageType
    {
        IMAGE_TYPE_1D,
        IMAGE_TYPE_2D,
        IMAGE_TYPE_3D,
        IMAGE_TYPE_CUBE,
        IMAGE_TYPE
    } ImageType;

    typedef enum ImageTiling
    {
        IMAGE_TILING_OPTIMAL,
        IMAGE_TILING_LINEAR,
        MAX_IMAGE_TILING
    } ImageTiling;

    typedef enum ImageUsageFlags
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
    } ImageUsageFlags;
    DECL_ENUM_AS_FLAGS(ImageUsageFlags)

        struct ImageCreateDesc
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

        ImageType				type;
        uint32_t				width;
        uint32_t				height;
        uint32_t				depth;
        uint32_t				arrayLayers;
        uint32_t				baseArrayLayer;
        uint32_t				mipLevels;
        uint32_t				baseMipLevel;
        uint32_t				sampleCount;
        uint32_t				sampleQuality;
        TextureFormat			format;
        ImageUsageFlags		    usage;
        ImageTiling			    tiling;
        MemoryType				memoryType;
        void* pData;
        size_t					dataSize;
        void* nativeSwapchainImage;
    };

    /*! \struct IImage
    \brief Interface for image resource. */
    struct IImage {
        virtual const ImageCreateDesc* get_desc() const = 0;
    };
}