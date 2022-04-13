#include "renderer-vulkan/vulkan_renderer.h"
#include "core/logging_macros.h"
#include "util/hash.h"
#include <iostream>
#include <set>

#undef max
#undef min

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include <algorithm>

#ifdef __linux__
#include "SDL_syswm.h"
#include <X11/Xlib-xcb.h> // for XGetXCBConnection()
#endif

size_t gMaxTextureId = 0;

constexpr VkImageType gImageTypeTranslator[ImageType::IMAGE_TYPE] =
{
    VK_IMAGE_TYPE_1D,
    VK_IMAGE_TYPE_2D,
    VK_IMAGE_TYPE_3D,
    VK_IMAGE_TYPE_2D,
};

constexpr VkImageTiling gImageTilingTranslator[ImageTiling::MAX_IMAGE_TILING] =
{
    VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_TILING_LINEAR
};

constexpr VkImageAspectFlags imageAspectFlagFromFormat(VkFormat format)
{
    VkImageAspectFlags result = 0;

    switch (format)
    {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
    case VK_FORMAT_D32_SFLOAT:
    {
        // Depth-only formats.
        result = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    break;
    case VK_FORMAT_S8_UINT:
    {
        // Stencil-only formats.
        result = VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    break;
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
    {
        // Depth stencil formats.
        result = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    break;
    default:
    {
        // If its not depth or depth/stencil, assume color.
        result = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    break;
    }

    return result;
}

constexpr VkSampleCountFlagBits textureSamplesToVkSamples(uint32_t samples)
{
    VkSampleCountFlagBits r = VK_SAMPLE_COUNT_1_BIT;

    switch (samples)
    {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
        r = (VkSampleCountFlagBits)samples;
        break;
    default:
        RLOG_WARNING("Unsupported sample count: (%i), defaulting to 1", samples);
        break;
    }

    return r;
}

VkImageUsageFlags textureUsageFlagsToVkImageFlags(ImageUsageFlags flags)
{
    VkImageUsageFlags rflags = 0;

    if ((flags & ImageUsageFlags::IMAGE_USAGE_COLOR_ATTACHMENT) != 0)
    {
        rflags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    if ((flags & ImageUsageFlags::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT) != 0)
    {
        rflags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    if ((flags & ImageUsageFlags::IMAGE_USAGE_INPUT_ATTACHMENT) != 0)
    {
        rflags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }

    if ((flags & ImageUsageFlags::IMAGE_USAGE_SAMPLED) != 0)
    {
        rflags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    if ((flags & ImageUsageFlags::IMAGE_USAGE_STORAGE) != 0)
    {
        rflags |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    if ((flags & ImageUsageFlags::IMAGE_USAGE_TRANSFER_DST) != 0)
    {
        rflags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if ((flags & ImageUsageFlags::IMAGE_USAGE_TRANSFER_SRC) != 0)
    {
        rflags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if ((flags & ImageUsageFlags::IMAGE_USAGE_TRANSIENT_ATTACHMENT) != 0)
    {
        rflags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    }

    return rflags;
}

VkMemoryPropertyFlags translateMemoryPropertyFlagsToVk(MemoryPropertyFlags flags)
{
    VkMemoryPropertyFlags rflags = 0;

    if ((flags & MemoryPropertyFlags::MEMORY_PROPERTY_DEVICE_LOCAL) != 0)
    {
        rflags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }

    if ((flags & MemoryPropertyFlags::MEMORY_PROPERTY_HOST_VISIBLE) != 0)
    {
        rflags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }

    if ((flags & MemoryPropertyFlags::MEMORY_PROPERTY_HOST_COHERENT) != 0)
    {
        rflags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    if ((flags & MemoryPropertyFlags::MEMORY_PROPERTY_HOST_CACHED) != 0)
    {
        rflags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    }

    return rflags;
}

constexpr VkFormat gTextureFormatTranslator[TextureFormat::MAX_TEXTURE_FORMAT] =
{
    VK_FORMAT_UNDEFINED,

    VK_FORMAT_R32G32B32A32_SFLOAT,
    VK_FORMAT_R32G32B32A32_UINT,
    VK_FORMAT_R32G32B32A32_SINT,

    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32_UINT,
    VK_FORMAT_R32G32B32_SINT,

    VK_FORMAT_R16G16B16A16_UNORM,
    VK_FORMAT_R16G16B16A16_SFLOAT,
    VK_FORMAT_R16G16B16A16_UNORM,
    VK_FORMAT_R16G16B16A16_UINT,
    VK_FORMAT_R16G16B16A16_SNORM,
    VK_FORMAT_R16G16B16A16_SINT,

    VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R32G32_UINT,
    VK_FORMAT_R32G32_SINT,

    VK_FORMAT_D32_SFLOAT_S8_UINT,

    VK_FORMAT_A2R10G10B10_UNORM_PACK32,
    VK_FORMAT_A2R10G10B10_UNORM_PACK32,
    VK_FORMAT_A2R10G10B10_UINT_PACK32,
    VK_FORMAT_UNDEFINED,

    VK_FORMAT_R8G8B8_UNORM,
    VK_FORMAT_R8G8B8_UNORM,
    VK_FORMAT_R8G8B8_SRGB,
    VK_FORMAT_R8G8B8_UINT,
    VK_FORMAT_R8G8B8_SNORM,
    VK_FORMAT_R8G8B8_SINT,

    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_R8G8B8A8_UINT,
    VK_FORMAT_R8G8B8A8_SNORM,
    VK_FORMAT_R8G8B8A8_SINT,

    VK_FORMAT_R16G16_UNORM,
    VK_FORMAT_R16G16_SFLOAT,
    VK_FORMAT_R16G16_UNORM,
    VK_FORMAT_R16G16_UINT,
    VK_FORMAT_R16G16_SNORM,
    VK_FORMAT_R16G16_SINT,

    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_R32_SFLOAT,
    VK_FORMAT_R32_UINT,
    VK_FORMAT_R32_SINT,

    VK_FORMAT_D24_UNORM_S8_UINT,

    VK_FORMAT_R8G8_UNORM,
    VK_FORMAT_R8G8_UNORM,
    VK_FORMAT_R8G8_UINT,
    VK_FORMAT_R8G8_SNORM,
    VK_FORMAT_R8G8_SINT,

    VK_FORMAT_R16_UNORM,
    VK_FORMAT_R16_SFLOAT,
    VK_FORMAT_D16_UNORM,
    VK_FORMAT_R16_UNORM,
    VK_FORMAT_R16_UINT,
    VK_FORMAT_R16_SNORM,
    VK_FORMAT_R16_SINT,

    VK_FORMAT_R8_UNORM,
    VK_FORMAT_R8_UNORM,
    VK_FORMAT_R8_UINT,
    VK_FORMAT_R8_SNORM,
    VK_FORMAT_R8_SINT,

    VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
    VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
    VK_FORMAT_BC1_RGBA_SRGB_BLOCK,

    VK_FORMAT_BC2_UNORM_BLOCK,
    VK_FORMAT_BC2_UNORM_BLOCK,
    VK_FORMAT_BC2_SRGB_BLOCK,

    VK_FORMAT_BC3_UNORM_BLOCK,
    VK_FORMAT_BC3_UNORM_BLOCK,
    VK_FORMAT_BC3_SRGB_BLOCK,

    VK_FORMAT_BC4_UNORM_BLOCK,
    VK_FORMAT_BC4_UNORM_BLOCK,
    VK_FORMAT_BC4_SNORM_BLOCK,

    VK_FORMAT_BC5_UNORM_BLOCK,
    VK_FORMAT_BC5_UNORM_BLOCK,
    VK_FORMAT_BC5_SNORM_BLOCK,

    VK_FORMAT_B8G8R8A8_UNORM,
    VK_FORMAT_B8G8R8A8_UNORM,
    VK_FORMAT_B8G8R8A8_SRGB,

    VK_FORMAT_BC6H_UFLOAT_BLOCK,
    VK_FORMAT_BC6H_SFLOAT_BLOCK,

    VK_FORMAT_BC7_UNORM_BLOCK,
    VK_FORMAT_BC7_UNORM_BLOCK,
    VK_FORMAT_BC7_SRGB_BLOCK
};

TextureFormat VkFormatToGenericFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R8G8B8A8_UNORM:
        return TEXTURE_FORMAT_RGBA8_UNORM;
        break;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return TEXTURE_FORMAT_RGBA8_UNORM_SRGB;
        break;
    case VK_FORMAT_R8G8B8A8_UINT:
        return TEXTURE_FORMAT_RGBA8_UINT;
        break;
    case VK_FORMAT_R8G8B8A8_SNORM:
        return TEXTURE_FORMAT_RGBA8_SNORM;
        break;
    case VK_FORMAT_R8G8B8A8_SINT:
        return TEXTURE_FORMAT_RGBA8_SINT;
        break;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return TEXTURE_FORMAT_BGRA8_UNORM;
        break;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return TEXTURE_FORMAT_BGRA8_SRGB;
        break;
    case VK_FORMAT_B8G8R8A8_UINT:
        return TEXTURE_FORMAT_BGRA8;
        break;
    case VK_FORMAT_B8G8R8A8_SNORM:
        return TEXTURE_FORMAT_BGRA8;
        break;
    case VK_FORMAT_B8G8R8A8_SINT:
        return TEXTURE_FORMAT_BGRA8;
        break;
    default:
        RLOG_WARNING("Unsupported backbuffer format, the format in render target will default to RGBA8");
        return TEXTURE_FORMAT_RGBA8;
        break;
    }

    return TEXTURE_FORMAT_UNKNOWN;
}

constexpr VkSamplerAddressMode gSamplerAddressModeTranslator[SamplerAddressMode::MAX_SAMPLER_ADDRESS_MODE] =
{
    VK_SAMPLER_ADDRESS_MODE_REPEAT,
    VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
};

constexpr VkBorderColor gBorderColorTranslator[BorderColor::MAX_BORDER_COLOR] =
{
    VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
    VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
    VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
    VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
    VK_BORDER_COLOR_INT_OPAQUE_WHITE
};

constexpr VkCompareOp gSamplerComparisonFuncTranslator[SamplerComparisonFunc::MAX_SAMPLER_COMPARISON] =
{
    VK_COMPARE_OP_NEVER,
    VK_COMPARE_OP_GREATER,
    VK_COMPARE_OP_GREATER_OR_EQUAL,
    VK_COMPARE_OP_LESS,
    VK_COMPARE_OP_LESS_OR_EQUAL,
    VK_COMPARE_OP_EQUAL
};

constexpr VkFilter gSamplerFilterTranslator[SamplerFilter::MAX_SAMPLER_FILTER] =
{
    VK_FILTER_LINEAR,
    VK_FILTER_NEAREST
};

constexpr VkSamplerMipmapMode gMipModeTranslator[MipMode::MAX_MIP_MODE] =
{
    VK_SAMPLER_MIPMAP_MODE_NEAREST,
    VK_SAMPLER_MIPMAP_MODE_LINEAR
};

constexpr VkShaderStageFlagBits gShaderStageTranslator[ShaderStage::MAX_SHADER_STAGE] =
{
    VK_SHADER_STAGE_VERTEX_BIT,
    VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    VK_SHADER_STAGE_GEOMETRY_BIT,
    VK_SHADER_STAGE_FRAGMENT_BIT,
    VK_SHADER_STAGE_COMPUTE_BIT,
};

constexpr VkVertexInputRate gInputRateTranslator[VertexInputRate::MAX_VERTEX_INPUT_RATE] =
{
    VK_VERTEX_INPUT_RATE_VERTEX,
    VK_VERTEX_INPUT_RATE_INSTANCE
};

constexpr VkIndexType gIndexTypeTranslator[IndexType::MAX_INDEX_TYPE] =
{
    VK_INDEX_TYPE_UINT16,
    VK_INDEX_TYPE_UINT32
};

/*! Enumeration describing a descriptor type. */
constexpr VkDescriptorType gDescriptorTypeTranslator[DescriptorType::MAX_DESCRIPTOR_TYPE]
{
    VK_DESCRIPTOR_TYPE_SAMPLER,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
};

constexpr VkPipelineBindPoint gPipelineTypeTranslator[PipelineType::MAX_PIPELINE_TYPE]
{
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    VK_PIPELINE_BIND_POINT_COMPUTE
};

VkShaderStageFlags shaderStageFlagsToVkShaderStageFlags(ShaderStageFlags flags)
{
    if (flags == SHADER_STAGE_FLAG_ALL)
    {
        return VK_SHADER_STAGE_ALL;
    }

    VkShaderStageFlags rflags = 0;

    if (flags == SHADER_STAGE_FLAG_ALL_GRAPHICS)
    {
        rflags = VK_SHADER_STAGE_ALL_GRAPHICS;
    }

    if ((flags & SHADER_STAGE_FLAG_VERTEX) != 0)
    {
        rflags |= VK_SHADER_STAGE_VERTEX_BIT;
    }

    if ((flags & SHADER_STAGE_FLAG_TESSELLATION_CONTROL) != 0)
    {
        rflags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    }

    if ((flags & SHADER_STAGE_FLAG_TESSELLATION_EVALUATION) != 0)
    {
        rflags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }

    if ((flags & SHADER_STAGE_FLAG_GEOMETRY) != 0)
    {
        rflags |= VK_SHADER_STAGE_GEOMETRY_BIT;
    }

    if ((flags & SHADER_STAGE_FLAG_FRAGMENT) != 0)
    {
        rflags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    if ((flags & SHADER_STAGE_FLAG_COMPUTE) != 0)
    {
        rflags |= VK_SHADER_STAGE_COMPUTE_BIT;
    }

    return rflags;
};

VkAccessFlags translateResourceStateToAccessMask(ResourceState state)
{
    VkAccessFlags rflags = (VkAccessFlags)0;

    if (state & RESOURCE_STATE_COPY_SOURCE) {
        rflags |= VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (state & RESOURCE_STATE_COPY_DEST) {
        rflags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (state & RESOURCE_STATE_VERTEX_BUFFER) {
        rflags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    }
    if (state & RESOURCE_STATE_UNIFORM_BUFFER) {
        rflags |= VK_ACCESS_UNIFORM_READ_BIT;
    }
    if (state & RESOURCE_STATE_INDEX_BUFFER) {
        rflags |= VK_ACCESS_INDEX_READ_BIT;
    }
    if (state & RESOURCE_STATE_UNORDERED_ACCESS) {
        rflags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    }
    if (state & RESOURCE_STATE_INDIRECT_ARGUMENT) {
        rflags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    }
    if (state & RESOURCE_STATE_RENDER_TARGET) {
        rflags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (state & RESOURCE_STATE_DEPTH_WRITE) {
        rflags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    if (state & RESOURCE_STATE_DEPTH_READ) {
        rflags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }
    if ((state & RESOURCE_STATE_SHADER_RESOURCE) || (state & RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)) {
        rflags |= VK_ACCESS_SHADER_READ_BIT;
    }
    if (state & RESOURCE_STATE_PRESENT) {
        rflags |= VK_ACCESS_MEMORY_READ_BIT;
    }

    return rflags;
}

VkImageLayout translateResourceStateToImageLayout(ResourceState state)
{
    if ((state & RESOURCE_STATE_COPY_SOURCE) != 0)
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    if ((state & RESOURCE_STATE_COPY_DEST) != 0)
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    if ((state & RESOURCE_STATE_RENDER_TARGET) != 0)
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    if ((state & RESOURCE_STATE_DEPTH_READ) != 0)
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    if ((state & RESOURCE_STATE_DEPTH_WRITE) != 0)
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    if ((state & RESOURCE_STATE_UNORDERED_ACCESS) != 0)
        return VK_IMAGE_LAYOUT_GENERAL;

    if ((state & RESOURCE_STATE_SHADER_RESOURCE) != 0)
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if ((state & RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE) != 0)
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if ((state & RESOURCE_STATE_PRESENT) != 0)
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    if ((state == RESOURCE_STATE_COMMON) != 0)
        return VK_IMAGE_LAYOUT_GENERAL;

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

constexpr VkCommandBufferLevel gCommandListTypeTranslator[CommandListType::MAX_COMMAND_LIST] =
{
    VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    VK_COMMAND_BUFFER_LEVEL_SECONDARY
};

constexpr VkPrimitiveTopology gPrimitiveTopologyTranslator[PrimitiveTopology::MAX_PRIMITIVE_TOPOLOGY] =
{
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
};

constexpr VkPolygonMode gPolygonModeTranslator[PolygonMode::MAX_POLYGON_MODE] =
{
    VK_POLYGON_MODE_FILL,
    VK_POLYGON_MODE_LINE,
    VK_POLYGON_MODE_POINT
};

constexpr VmaMemoryUsage gMemoryTypeTranslator[MemoryType::MEMORY_TYPE_MAX] =
{
    VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
    VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY,
    VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
    VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_TO_CPU,
};

constexpr VkCullModeFlags gCullModeTranslator[CullMode::MAX_CULL_MODE] =
{
    VK_CULL_MODE_NONE,
    VK_CULL_MODE_FRONT_BIT,
    VK_CULL_MODE_BACK_BIT,
    VK_CULL_MODE_FRONT_AND_BACK
};

constexpr VkFrontFace gFrontFaceTranslator[FrontFaceWinding::MAX_FRONT_FACE_WINDING] =
{
    VK_FRONT_FACE_COUNTER_CLOCKWISE,
    VK_FRONT_FACE_CLOCKWISE
};

constexpr VkBlendFactor gBlendFactorTranslator[BlendFactor::MAX_BLEND_FACTOR] =
{
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_SRC_COLOR,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    VK_BLEND_FACTOR_DST_COLOR,
    VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_FACTOR_DST_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    VK_BLEND_FACTOR_CONSTANT_COLOR,
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
    VK_BLEND_FACTOR_CONSTANT_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
    VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
    VK_BLEND_FACTOR_SRC1_COLOR,
    VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
    VK_BLEND_FACTOR_SRC1_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
};

constexpr VkBlendOp gBlendOpTranslator[BlendOp::MAX_BLEND_OP] =
{
    VK_BLEND_OP_ADD,
    VK_BLEND_OP_SUBTRACT,
    VK_BLEND_OP_REVERSE_SUBTRACT,
    VK_BLEND_OP_MIN,
    VK_BLEND_OP_MAX
};

constexpr VkLogicOp gLogicOpTranslator[LogicOp::MAX_LOGIC_OP] =
{
    VK_LOGIC_OP_CLEAR,
    VK_LOGIC_OP_AND,
    VK_LOGIC_OP_AND_REVERSE,
    VK_LOGIC_OP_COPY,
    VK_LOGIC_OP_AND_INVERTED,
    VK_LOGIC_OP_NO_OP,
    VK_LOGIC_OP_XOR,
    VK_LOGIC_OP_OR,
    VK_LOGIC_OP_NOR,
    VK_LOGIC_OP_EQUIVALENT,
    VK_LOGIC_OP_INVERT,
    VK_LOGIC_OP_OR_REVERSE,
    VK_LOGIC_OP_COPY_INVERTED,
    VK_LOGIC_OP_OR_INVERTED,
    VK_LOGIC_OP_NAND,
    VK_LOGIC_OP_SET
};

constexpr VkCompareOp gCompareOpTranslator[CompareOp::MAX_COMPARE_OP] =
{
    VK_COMPARE_OP_NEVER,
    VK_COMPARE_OP_LESS,
    VK_COMPARE_OP_EQUAL,
    VK_COMPARE_OP_LESS_OR_EQUAL,
    VK_COMPARE_OP_GREATER,
    VK_COMPARE_OP_NOT_EQUAL,
    VK_COMPARE_OP_GREATER_OR_EQUAL,
    VK_COMPARE_OP_ALWAYS
};

constexpr VkAttachmentLoadOp gLoadOpTranslator[LoadOp::MAX_LOAD_OP] =
{
    VK_ATTACHMENT_LOAD_OP_LOAD,
    VK_ATTACHMENT_LOAD_OP_CLEAR,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE
};

constexpr VkAttachmentStoreOp gStoreOpTranslator[StoreOp::MAX_STORE_OP] =
{
    VK_ATTACHMENT_STORE_OP_STORE,
    VK_ATTACHMENT_STORE_OP_DONT_CARE,

};

constexpr VkStencilOp gStencilOpTranslator[StencilOp::MAX_STENCIL_OP] =
{
    VK_STENCIL_OP_KEEP,
    VK_STENCIL_OP_ZERO,
    VK_STENCIL_OP_REPLACE,
    VK_STENCIL_OP_INCREMENT_AND_CLAMP,
    VK_STENCIL_OP_DECREMENT_AND_CLAMP,
    VK_STENCIL_OP_INVERT,
    VK_STENCIL_OP_INCREMENT_AND_WRAP,
    VK_STENCIL_OP_DECREMENT_AND_WRAP
};

VkBufferUsageFlags bufferUsageFlagsToVkBufferFlags(BufferUsageFlags flags)
{
    VkBufferUsageFlags rflags = 0;

    if ((flags & BUFFER_USAGE_TRANSFER_SRC) != 0)
    {
        rflags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }

    if ((flags & BUFFER_USAGE_TRANSFER_DST) != 0)
    {
        rflags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    if ((flags & BUFFER_USAGE_UNIFORM_TEXEL_BUFFER) != 0)
    {
        rflags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
    }

    if ((flags & BUFFER_USAGE_STORAGE_TEXEL_BUFFER) != 0)
    {
        rflags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
    }

    if ((flags & BUFFER_USAGE_UNIFORM_BUFFER) != 0)
    {
        rflags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }

    if ((flags & BUFFER_USAGE_STORAGE_BUFFER) != 0)
    {
        rflags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    if ((flags & BUFFER_USAGE_INDEX_BUFFER) != 0)
    {
        rflags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    if ((flags & BUFFER_USAGE_VERTEX_BUFFER) != 0)
    {
        rflags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }

    if ((flags & BUFFER_USAGE_INDIRECT_BUFFER) != 0)
    {
        rflags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    }

    return rflags;
}

RenderResult VulkanRenderer::initialize(RendererInitializationDesc* pDesc)
{
    // Don't try to initialize if we already successfully initialized before.
    if (is_initialized())
    {
        return RR_ALREADY_INITIALIZED;
    }

    // Local variable for storing results of initialization functions.
    RenderResult result;

    // Create a vulkan instance.
    result = create_vulkan_instance(pDesc);
    if (result != RR_SUCCESS)
    {
        return result;
    }

    memcpy(&_renderer_info, pDesc, sizeof(RendererInitializationDesc));

    // Set up debugging callbacks
    if (pDesc->enableDebugging)
    {
        result = setup_debug_callback();
        if (result != RR_SUCCESS)
        {
            return result;
        }
    }

    // Query all available GPU's and store them in the _gpus vector.
    result = query_available_physical_devices();
    if (result != RR_SUCCESS)
    {
        return result;
    }

    // Pick most suitable physical device.
    VulkanPhysicalDevice* physicalDevice;
    result = pick_physical_device(&physicalDevice);
    if (result != RR_SUCCESS)
    {
        return result;
    }

    // Create logical device.
    result = create_logical_device(physicalDevice);
    if (result != RR_SUCCESS)
    {
        return result;
    }

    // Create allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _device->physicalDevice->physicalDevice;
    allocatorInfo.device = _device->device;

    vmaCreateAllocator(&allocatorInfo, &_allocator);

    // The renderer has been properly initialized, therefore we can set the initialized member to true.
    _initialized = true;

    // Create transfer queue.
    QueueCreateDesc queueDesc;
    queueDesc.type = CommandType::COMMAND_TYPE_COPY;

    result = create_queue((IQueue **)& _transfer_queue, &queueDesc);
    if (result != RR_SUCCESS)
    {
        _initialized = false;
        return result;
    }

    result = create_command_pool((ICommandPool **)& _transfer_command_pool, _transfer_queue, nullptr);
    if (result != RR_SUCCESS)
    {
        _initialized = false;
        return result;
    }

    queueDesc.type = CommandType::COMMAND_TYPE_DIRECT;

    result = create_queue((IQueue **)& _graphics_queue, &queueDesc);
    if (result != RR_SUCCESS)
    {
        _initialized = false;
        return result;
    }

    result = create_command_pool((ICommandPool **)& _graphics_command_pool, _graphics_queue, nullptr);
    if (result != RR_SUCCESS)
    {
        _initialized = false;
        return result;
    }

    return RR_SUCCESS;
}

bool VulkanRenderer::is_initialized() const
{
    return _initialized;
}

RenderResult VulkanRenderer::create_vulkan_instance(RendererInitializationDesc* pDesc)
{
    // Define vulkan application. Useful for driver optimizations (aka useless for small applications)
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = pDesc->applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(pDesc->versionMajor, pDesc->versionMinor, pDesc->versionPatch);
    appInfo.pEngineName = "Raven";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    // Vulkan instance creation info, pass the application information as argument.
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.enabledExtensionCount = 0;

    // Define validation layers we want to have attached to our instance.
    Vector<const char*> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
    };

    // We always require the surface extension to be present, and an surface extension for our platform.
    Vector<const char*> instanceExtensions;
    instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(PLATFORM_WINDOWS)
    instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(PLATFORM_LINUX)
    instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

    // Add required instance extensions to instance extension check vector.
    if (!pDesc->requiredVulkanInstanceExtensions.empty())
    {
        for (auto extension : pDesc->requiredVulkanInstanceExtensions)
        {
            instanceExtensions.push_back(extension.c_str());
        }

    }

    // Enable debug report extensions if we're running in debug mode.
    if (pDesc->enableDebugging)
    {
        instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

        if (!pDesc->requiredVulkanInstanceLayers.empty())
        {
            for (auto layer : pDesc->requiredVulkanInstanceLayers)
            {
                validationLayers.push_back(layer.c_str());
            }
        }

        if (enable_validation_layers(createInfo, validationLayers) != RR_SUCCESS)
        {
            return RR_LAYER_NOT_PRESENT;
        }
    }

    // Enable extensions
    if (enable_instance_extensions(createInfo, instanceExtensions) != RR_SUCCESS)
    {
        return RR_EXTENSION_NOT_PRESENT;
    }

    // Create the vulkan instance using our vulkan create info structure. We don't define an allocator for now.
    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
        return RR_INITIALIZATION_FAILED;
    }

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::enable_validation_layers(VkInstanceCreateInfo& rInfo, const Vector<const char*>& rInstanceLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    // Retrieve available layers into the availablelayers vector.
    Vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // For each layer that we want enabled, check if we support it.
    for (const char* layerName : rInstanceLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return RR_LAYER_NOT_PRESENT;
        }
    }


    rInfo.enabledLayerCount = static_cast<uint32_t>(rInstanceLayers.size());
    rInfo.ppEnabledLayerNames = rInstanceLayers.data();

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::enable_instance_extensions(VkInstanceCreateInfo& rInfo, const Vector<const char*>& rInstanceExtensions)
{
    // Query how many extensions are available to us.
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    // Container of extensions.
    Vector<VkExtensionProperties> extensions(extensionCount);

    // Retrieve the extensions we have available into our extensions vector.
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    // For each extension that we want enabled, check if we support it.
    for (const char* extensionName : rInstanceExtensions) {
        bool extFound = false;

        for (const auto& extProperty : extensions) {
            if (strcmp(extensionName, extProperty.extensionName) == 0) {
                extFound = true;
                break;
            }
        }

        if (!extFound) {
            return RR_EXTENSION_NOT_PRESENT;
        }
    }

    // Add the extensions to the create info structure.
    rInfo.enabledExtensionCount = static_cast<uint32_t>(rInstanceExtensions.size());
    rInfo.ppEnabledExtensionNames = rInstanceExtensions.data();

    return RR_SUCCESS;
}

VkResult VulkanRenderer::create_debug_report_callback_extension(VkInstance pInstance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    auto func = GET_VK_FUNC(_instance, vkCreateDebugReportCallbackEXT);
    if (func != nullptr) {
        return func(_instance, pCreateInfo, pAllocator, pCallback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
{
    RLOG_WARNING("Vulkan Validation: %s", msg);
    return VK_FALSE;
}

RenderResult VulkanRenderer::destroy_debug_report_callback_extension(VkInstance pInstance, VkDebugReportCallbackEXT pCallback, const VkAllocationCallbacks* pAllocator) {
    auto func = GET_VK_FUNC(_instance, vkDestroyDebugReportCallbackEXT);
    if (func != nullptr) {
        func(_instance, pCallback, pAllocator);
        return RR_SUCCESS;
    }

    return RR_EXTENSION_NOT_PRESENT;
}

RenderResult VulkanRenderer::query_swap_chain_support(VulkanDevice* pDevice, VulkanSwapChain* pSwapChain)
{
    // Retrieve surface capabilities.
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice->physicalDevice->physicalDevice, pSwapChain->surface, &pSwapChain->surfaceCapabilities);

    // Retrieve surface formats for the created surface.
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice->physicalDevice->physicalDevice, pSwapChain->surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        pSwapChain->surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice->physicalDevice->physicalDevice, pSwapChain->surface, &formatCount, pSwapChain->surfaceFormats.data());
    }
    else
    {
        RLOG_ERROR("The specified surface has no surface formats.");
        return RR_FEATURE_NOT_PRESENT;
    }

    // Retrieve present modes for this surface.
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice->physicalDevice->physicalDevice, pSwapChain->surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        pSwapChain->surfacePresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice->physicalDevice->physicalDevice, pSwapChain->surface, &presentModeCount, pSwapChain->surfacePresentModes.data());
    }
    else
    {
        RLOG_ERROR("The specified surface has no present modes.");
        return RR_FEATURE_NOT_PRESENT;
    }

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::choose_swap_chain_surface_format(VulkanSwapChain* pSwapChain)
{
    RASSERT(pSwapChain->surfaceFormats.size() != 0);

    // If we can choose the format, set it to standard SRGB.
    if (pSwapChain->surfaceFormats.size() == 1 && pSwapChain->surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        pSwapChain->surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM,  VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        return RR_SUCCESS;
    }

    // Otherwise, go through the available formats and pick the first format that has the format and color space we want.
    for (const auto& availableFormat : pSwapChain->surfaceFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            pSwapChain->surfaceFormat = availableFormat;
            return RR_SUCCESS;
        }
    }

    // If all else fails, we'll use the first format.
    pSwapChain->surfaceFormat = pSwapChain->surfaceFormats[0];
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::choose_swap_chain_present_mode(VulkanSwapChain* pSwapChain)
{
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    // Mailbox allows for triple buffering with lower latency issues compared to traditional FIFO (vsync) double buffering.
    for (const auto& availablePresentMode : pSwapChain->surfacePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            pSwapChain->surfacePresentMode = availablePresentMode;
            return RR_SUCCESS;
        }
        else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            bestMode = availablePresentMode;
        }
    }

    pSwapChain->surfacePresentMode = bestMode;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::choose_swap_chain_extent(VulkanSwapChain* pSwapChain)
{
    if (pSwapChain->surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        pSwapChain->swapChainExtent = pSwapChain->surfaceCapabilities.currentExtent;
        return RR_SUCCESS;
    }
    else
    {
        VkExtent2D actualExtent = { 1024, 896 };

        actualExtent.width = std::max(pSwapChain->surfaceCapabilities.minImageExtent.width,
            std::min(pSwapChain->surfaceCapabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(pSwapChain->surfaceCapabilities.minImageExtent.height,
            std::min(pSwapChain->surfaceCapabilities.maxImageExtent.height, actualExtent.height));

        pSwapChain->swapChainExtent = actualExtent;

        return RR_SUCCESS;
    }
}

RenderResult VulkanRenderer::create_window_surface(Window* pWindow, VkSurfaceKHR* pSurface)
{
#if defined(PLATFORM_WINDOWS)
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = (HWND)pWindow->get_handle();
    createInfo.hinstance = GetModuleHandle(nullptr);

    auto CreateWin32SurfaceKHR = GET_VK_FUNC(_instance, vkCreateWin32SurfaceKHR);

    if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(_instance, &createInfo, nullptr, pSurface) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create window surface!");
        return RR_INITIALIZATION_FAILED;
    }

#elif defined(__linux__)
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (!SDL_GetWindowWMInfo((SDL_Window*)pWindow->get_handle(), &wminfo))
        return RR_OBJECT_DOES_NOT_EXIST;

    if (wminfo.subsystem != SDL_SYSWM_X11)
    {
        RLOG_ERROR("We can only initialize an X11 surface!");
        return RR_INITIALIZATION_FAILED;
    }

    VkXcbSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.window = wminfo.info.x11.window;
    createInfo.connection = XGetXCBConnection(wminfo.info.x11.display);

    auto CreateXcbSurfaceKHR = GET_VK_FUNC(_instance, vkCreateXcbSurfaceKHR);
    if (!CreateXcbSurfaceKHR || CreateXcbSurfaceKHR(_instance, &createInfo, nullptr, pSurface) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create window surface!");
        return RR_INITIALIZATION_FAILED;
    }
#endif
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_swap_chain(Window* pNativeWindow, ISwapChain** ppSwapChain)
{
    RASSERT(_initialized);
    RASSERT(_device != nullptr);

    VulkanSwapChain* swapChain = new VulkanSwapChain();

    // Create surface for window.
    if (create_window_surface(pNativeWindow, &swapChain->surface) != RR_SUCCESS)
    {
        RLOG_ERROR("Failed to create surface for the specified window.");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }


    VkBool32 deviceSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(_device->physicalDevice->physicalDevice, _device->queueIndices.graphicsQueueIndex, swapChain->surface, &deviceSupport);
    if (deviceSupport != VK_TRUE)
    {
        return RR_INITIALIZATION_FAILED;
    }

    // Retrieve swapchain support details into the surface object.
    if (query_swap_chain_support(_device, swapChain) != RR_SUCCESS)
    {
        RLOG_ERROR("We do not support swapchain creation for this device (Missing extensions?).");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }

    if (choose_swap_chain_surface_format(swapChain) != RR_SUCCESS)
    {
        RLOG_ERROR("We could not find a suitable SwapChain surface format.");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }

    if (choose_swap_chain_present_mode(swapChain) != RR_SUCCESS)
    {
        RLOG_ERROR("We could not find a suitable SwapChain present mode.");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }

    if (choose_swap_chain_extent(swapChain) != RR_SUCCESS)
    {
        RLOG_ERROR("We could not find a suitable SwapChain surface format.");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }

    // Select how many backbuffers we need in the swap chain, we preferably want the minimum amount + 1 for minimal latency without tearing.
    uint32_t imageCount = swapChain->surfaceCapabilities.minImageCount + 1;
    if (swapChain->surfaceCapabilities.maxImageCount > 0 && imageCount > swapChain->surfaceCapabilities.maxImageCount)
    {
        imageCount = swapChain->surfaceCapabilities.maxImageCount;
    }

    // Fill the swapchain creation info structure.
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = swapChain->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = swapChain->surfaceFormat.format;
    createInfo.imageColorSpace = swapChain->surfaceFormat.colorSpace;
    createInfo.imageExtent = swapChain->swapChainExtent;
    createInfo.imageArrayLayers = 1; // no VR support.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    // If the present queue and graphics queue are not on the same queue, set the swapchain images to concurrect sharing.
    //if (graphicsIndex != presentIndex) {
    //	createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    //	createInfo.queueFamilyIndexCount = 2;
    //	createInfo.pQueueFamilyIndices = queueFamilyIndices;
    //}
    //else
    //{ // If the present queue and graphics queue ARE on the same queue, only one queue can have ownership at a time, and ownership has to be transferred to the other before presenting or rendering.
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
    //}

    createInfo.preTransform = swapChain->surfaceCapabilities.currentTransform; // We don't want any transformation so we set it to the current transform.
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // We don't want to use the alpha bit for blending with other windows so set it to OPAQUE.
    createInfo.presentMode = swapChain->surfacePresentMode; // Set the present mode to the one we selected.
    createInfo.clipped = VK_TRUE; // We don't need to read the swapchain image, so if another window is obscuring it, we don't care about it's colours.
    createInfo.oldSwapchain = VK_NULL_HANDLE; // For now we only need 1 swapchain ever and dont care.

    if (vkCreateSwapchainKHR(_device->device, &createInfo, nullptr, &swapChain->swapChain) != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to create SwapChain object.");
    }

    Vector<VkImage> swapChainImages; /*!< Vector containing a VkImage for each backbuffer in the SwapChain. */

    // Retrieve swap chain images.
    vkGetSwapchainImagesKHR(_device->device, swapChain->swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device->device, swapChain->swapChain, &imageCount, swapChainImages.data());

    swapChain->numBackbuffers = imageCount;
    swapChain->renderTargets = (VulkanRenderTarget**)malloc(sizeof(VulkanRenderTarget*)* imageCount);

    // Assign the native window to this swapchain.
    swapChain->window = pNativeWindow;

    for (uint32_t i = 0; i < swapChainImages.size(); i++)
    {
        RenderTargetCreateDesc rtDesc = {};
        rtDesc.format = VkFormatToGenericFormat(swapChain->surfaceFormat.format);
        rtDesc.width = swapChain->swapChainExtent.width;
        rtDesc.height = swapChain->swapChainExtent.height;
        rtDesc.sampleCount = 1;
        rtDesc.sampleQuality = 0;
        rtDesc.usage = RenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT;
        rtDesc.nativeSwapchainImage = (void*)swapChainImages[i];
        create_render_target((IRenderTarget **)(&swapChain->renderTargets[i]), &rtDesc);
    }

    create_semaphore((ISemaphore**)&swapChain->imageAcquiredSemaphore);

    // Update the pointer argument to point to the newly created swapchain.
    *ppSwapChain = (ISwapChain*)swapChain;

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_swap_chain(ISwapChain* pSwapchain)
{
    RASSERT(_device != nullptr);
    RASSERT(pSwapchain != nullptr);

    VulkanSwapChain* swapChain = static_cast<VulkanSwapChain*>(pSwapchain);

    for (uint32_t i = 0; i < swapChain->numBackbuffers; ++i)
    {
        vkDestroyImageView(_device->device, swapChain->renderTargets[i]->image->imageView, nullptr);
    }

    free(swapChain->renderTargets);
    vkDestroySwapchainKHR(_device->device, swapChain->swapChain, nullptr);
    vkDestroySurfaceKHR(_instance, swapChain->surface, nullptr);

    destroy_semaphore(swapChain->imageAcquiredSemaphore);

    delete pSwapchain;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::recreate_swap_chain(ISwapChain* pSwapchain)
{
    RASSERT(_device != nullptr);
    RASSERT(pSwapchain != nullptr);

    wait_for_device();

    VulkanSwapChain* swapChain = static_cast<VulkanSwapChain*>(pSwapchain);

    for (uint32_t i = 0; i < swapChain->numBackbuffers; ++i)
    {
        vkDestroyImageView(_device->device, swapChain->renderTargets[i]->image->imageView, nullptr);
    }

    free(swapChain->renderTargets);
    vkDestroySwapchainKHR(_device->device, swapChain->swapChain, nullptr);
    vkDestroySurfaceKHR(_instance, swapChain->surface, nullptr);

    destroy_semaphore(swapChain->imageAcquiredSemaphore);

    // Create surface for window.
    if (create_window_surface(swapChain->window, &swapChain->surface) != RR_SUCCESS)
    {
        RLOG_ERROR("Failed to create surface for the specified window.");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }


    VkBool32 deviceSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(_device->physicalDevice->physicalDevice, _device->queueIndices.graphicsQueueIndex, swapChain->surface, &deviceSupport);
    if (deviceSupport != VK_TRUE)
    {
        return RR_INITIALIZATION_FAILED;
    }

    // Retrieve swapchain support details into the surface object.
    if (query_swap_chain_support(_device, swapChain) != RR_SUCCESS)
    {
        RLOG_ERROR("We do not support swapchain creation for this device (Missing extensions?).");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }

    if (choose_swap_chain_surface_format(swapChain) != RR_SUCCESS)
    {
        RLOG_ERROR("We could not find a suitable SwapChain surface format.");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }

    if (choose_swap_chain_present_mode(swapChain) != RR_SUCCESS)
    {
        RLOG_ERROR("We could not find a suitable SwapChain present mode.");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }

    if (choose_swap_chain_extent(swapChain) != RR_SUCCESS)
    {
        RLOG_ERROR("We could not find a suitable SwapChain surface format.");
        delete swapChain;

        return RR_INITIALIZATION_FAILED;
    }

    // Select how many backbuffers we need in the swap chain, we preferably want the minimum amount + 1 for minimal latency without tearing.
    uint32_t imageCount = swapChain->surfaceCapabilities.minImageCount + 1;
    if (swapChain->surfaceCapabilities.maxImageCount > 0 && imageCount > swapChain->surfaceCapabilities.maxImageCount)
    {
        imageCount = swapChain->surfaceCapabilities.maxImageCount;
    }

    // Fill the swapchain creation info structure.
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = swapChain->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = swapChain->surfaceFormat.format;
    createInfo.imageColorSpace = swapChain->surfaceFormat.colorSpace;
    createInfo.imageExtent = swapChain->swapChainExtent;
    createInfo.imageArrayLayers = 1; // no VR support.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional

    createInfo.preTransform = swapChain->surfaceCapabilities.currentTransform; // We don't want any transformation so we set it to the current transform.
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // We don't want to use the alpha bit for blending with other windows so set it to OPAQUE.
    createInfo.presentMode = swapChain->surfacePresentMode; // Set the present mode to the one we selected.
    createInfo.clipped = VK_TRUE; // We don't need to read the swapchain image, so if another window is obscuring it, we don't care about it's colours.
    createInfo.oldSwapchain = VK_NULL_HANDLE; // For now we only need 1 swapchain ever and dont care.

    if (vkCreateSwapchainKHR(_device->device, &createInfo, nullptr, &swapChain->swapChain) != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to create SwapChain object.");
    }

    Vector<VkImage>	swapChainImages; /*!< Vector containing a VkImage for each backbuffer in the SwapChain. */

    // Retrieve swap chain images.
    vkGetSwapchainImagesKHR(_device->device, swapChain->swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device->device, swapChain->swapChain, &imageCount, swapChainImages.data());

    swapChain->numBackbuffers = imageCount;
    swapChain->renderTargets = (VulkanRenderTarget**)malloc(sizeof(VulkanRenderTarget*)* imageCount);


    for (uint32_t i = 0; i < swapChainImages.size(); i++)
    {
        RenderTargetCreateDesc rtDesc = {};
        rtDesc.format = VkFormatToGenericFormat(swapChain->surfaceFormat.format);
        rtDesc.width = swapChain->swapChainExtent.width;
        rtDesc.height = swapChain->swapChainExtent.height;
        rtDesc.sampleCount = 1;
        rtDesc.sampleQuality = 0;
        rtDesc.usage = RenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT;
        rtDesc.nativeSwapchainImage = (void*)swapChainImages[i];
        create_render_target((IRenderTarget * *)(&swapChain->renderTargets[i]), &rtDesc);
    }

    create_semaphore((ISemaphore **)&swapChain->imageAcquiredSemaphore);
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_queue(IQueue** ppQueue, QueueCreateDesc* pDesc)
{
    RASSERT(_initialized);
    RASSERT(_device != nullptr);
    RASSERT(pDesc != nullptr);

    VulkanQueue* queue = new VulkanQueue();

    uint32_t queueFamilyIndex = UINT32_MAX;

    // Retrieve queue family index.
    switch (pDesc->type)
    {
    case CommandType::COMMAND_TYPE_COMPUTE:
    {
        queueFamilyIndex = _device->queueIndices.computeQueueIndex;
    }
    break;
    case CommandType::COMMAND_TYPE_COPY:
    {
        queueFamilyIndex = _device->queueIndices.transferQueueIndex;
    }
    break;
    case CommandType::COMMAND_TYPE_DIRECT:
    case CommandType::COMMAND_TYPE_BUNDLE:
    {
        queueFamilyIndex = _device->queueIndices.graphicsQueueIndex;
    }
    break;
    default:
    {
        return RR_UNKNOWN_FAILURE;
    }
    break;
    }

    // We don't have a queue which supports this type of commands.
    if (queueFamilyIndex == UINT32_MAX)
    {
        return RR_NO_SUITABLE_DEVICE;
    }

    uint32_t queueIndex = 0;

    // If this is the first queue we get of this type, set the counter to 1 and use queue 0.
    if (_device->queueCounters.find(queueFamilyIndex) == _device->queueCounters.end())
    {
        _device->queueCounters[queueFamilyIndex] = 1;
    }
    else
    {
        // Get the first available queue index and increment the counter.
        queueIndex = _device->queueCounters[queueFamilyIndex];

        if (queueIndex >= _device->physicalDevice->queueFamilyProperties[queueFamilyIndex].queueCount)
        {
            queueIndex = _device->physicalDevice->queueFamilyProperties[queueFamilyIndex].queueCount - 1;
        }

        _device->queueCounters[queueFamilyIndex]++;
    }

    // Retrieve queue from the logical device.
    vkGetDeviceQueue(_device->device, queueFamilyIndex, queueIndex, &queue->vkQueue);

    queue->device = _device;
    queue->queueFamilyIndex = queueFamilyIndex;
    queue->queueIndex = queueIndex;

    // Set the out pointer to the newly created queue.
    *ppQueue = (IQueue*)queue;

    return RenderResult::RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_queue(IQueue* pQueue)
{
    VulkanQueue* queue = static_cast<VulkanQueue*>(pQueue);

    RASSERT(queue != nullptr);
    RASSERT(queue->device != nullptr);

    // Decrement queue index.
    queue->device->queueCounters[queue->queueFamilyIndex]--;

    delete pQueue;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_command_pool(ICommandPool** ppCmdPool, IQueue* pQueue, CommandPoolCreateDesc* pDesc)
{
    RASSERT(_initialized);
    RASSERT(_device != nullptr);

    VulkanQueue* queue = static_cast<VulkanQueue*>(pQueue);
    RASSERT(queue != nullptr);

    VulkanCommandPool* pool = new VulkanCommandPool();

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queue->queueFamilyIndex;
    poolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    // Create command pool for this queue.
    if (vkCreateCommandPool(_device->device, &poolInfo, nullptr, &pool->pool) != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to create command pool.");
        delete pool;

        return RR_INITIALIZATION_FAILED;
    }

    *ppCmdPool = (ICommandPool*)pool;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_command_pool(ICommandPool* pCmdPool)
{
    VulkanCommandPool* pool = static_cast<VulkanCommandPool*>(pCmdPool);
    RASSERT(pool != nullptr);

    vkDestroyCommandPool(_device->device, pool->pool, nullptr);

    delete pool;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_command_list(ICommandList** ppCommandList, ICommandPool* pCmdPool, CommandListType type)
{
    return create_command_lists(ppCommandList, 1, pCmdPool, type);
}

RenderResult VulkanRenderer::create_command_lists(ICommandList** ppCommandLists, uint32_t numCommandLists, ICommandPool* pCmdPool, CommandListType type)
{
    RASSERT(ppCommandLists != nullptr);
    RASSERT(pCmdPool != nullptr);

    VulkanCommandPool* pool = static_cast<VulkanCommandPool*>(pCmdPool);

    Vector<VkCommandBuffer> CommandBuffers;
    CommandBuffers.resize(numCommandLists);

    // Create command list wrapper objects.
    for (uint32_t i = 0; i < numCommandLists; ++i)
    {
        VulkanCommandList* newList = new VulkanCommandList();
        newList->boundPipeline = nullptr;
        newList->currentRenderPass = nullptr;
        newList->descriptorSetCache = nullptr;

        ppCommandLists[i] = newList;
    }

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool->pool;
    allocInfo.level = gCommandListTypeTranslator[type];
    allocInfo.commandBufferCount = (uint32_t)numCommandLists;

    if (vkAllocateCommandBuffers(_device->device, &allocInfo, CommandBuffers.data()) != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to allocate command buffers.");

        // Create command list wrapper objects.
        for (uint32_t i = 0; i < numCommandLists; ++i)
        {
            delete ppCommandLists[i];
            ppCommandLists[i] = nullptr;
        }
    }

    // Create command list wrapper objects.
    for (uint32_t i = 0; i < numCommandLists; ++i)
    {
        static_cast<VulkanCommandList*>(ppCommandLists[i])->cmdBuffer = CommandBuffers[i];
    }

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_command_list(ICommandPool* pCmdPool, ICommandList* pCommandList)
{
    return destroy_command_lists(pCmdPool, &pCommandList, 1);
}

RenderResult VulkanRenderer::destroy_command_lists(ICommandPool* pCmdPool, ICommandList** ppCommandLists, uint32_t numLists)
{
    RASSERT(pCmdPool != nullptr);
    RASSERT(ppCommandLists != nullptr);
    RASSERT(numLists > 0);

    VulkanCommandPool* pool = static_cast<VulkanCommandPool*>(pCmdPool);

    Vector<VkCommandBuffer> buffers;
    buffers.resize(numLists);

    for (uint32_t i = 0; i < numLists; ++i)
    {
        VulkanCommandList* list = static_cast<VulkanCommandList*>(ppCommandLists[i]);

        if (list->descriptorSetCache != nullptr)
        {
            destroy_descriptor_pool(list->descriptorSetCache->descriptorPool);
        }

        if (list->dynamicUniformAllocator != nullptr)
        {
            delete list->dynamicUniformAllocator;
        }

        buffers[i] = list->cmdBuffer;
        delete list;
    }

    vkFreeCommandBuffers(_device->device, pool->pool, numLists, buffers.data());
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_pipeline(IPipeline** ppPipeline, GraphicsPipelineCreateDesc* pDesc)
{
    RASSERT(pDesc != nullptr);
    RASSERT(_initialized);
    RASSERT(pDesc->pShader != nullptr);
    RASSERT(pDesc->pRootSignature != nullptr);
    RASSERT(pDesc->numRenderTargets <= 8);

    VulkanPipeline* pipeline = new VulkanPipeline();
    pipeline->type = PIPELINE_TYPE_GRAPHICS;

    // Create pipeline shader stages.
    VkPipelineShaderStageCreateInfo shaderStages[MAX_SHADER_STAGE];
    uint32_t numShaderStages = 0;

    VulkanShader* shader = static_cast<VulkanShader*>(pDesc->pShader);
    VulkanRootSignature* rootSignature = static_cast<VulkanRootSignature*>(pDesc->pRootSignature);

    for (uint32_t i = 0; i < MAX_SHADER_STAGE; i++)
    {
        if (shader->shaders[i] != VK_NULL_HANDLE)
        {
            VkPipelineShaderStageCreateInfo shaderStageInfo = {};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = gShaderStageTranslator[i];
            shaderStageInfo.module = shader->shaders[i];
            shaderStageInfo.pName = "main";

            shaderStages[numShaderStages] = shaderStageInfo;
            numShaderStages++;
        }
    }

    // Create input state info.
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(rootSignature->inputBindingDescriptions.size());
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(rootSignature->inputAttributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = rootSignature->inputBindingDescriptions.data();
    vertexInputInfo.pVertexAttributeDescriptions = rootSignature->inputAttributeDescriptions.data();

    // Create input assemply info.
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = gPrimitiveTopologyTranslator[pDesc->primitiveTopology];
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Define rasterizor.
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    if (pDesc->pRasterizerState != nullptr)
    {
        rasterizer.depthClampEnable = pDesc->pRasterizerState->depthClampEnable;
        rasterizer.rasterizerDiscardEnable = pDesc->pRasterizerState->rasterizerDiscardEnable;
        rasterizer.polygonMode = gPolygonModeTranslator[pDesc->pRasterizerState->polygonMode];
        rasterizer.lineWidth = pDesc->pRasterizerState->lineWidth;
        rasterizer.cullMode = gCullModeTranslator[pDesc->pRasterizerState->cullMode];
        rasterizer.frontFace = gFrontFaceTranslator[pDesc->pRasterizerState->frontFace];
        rasterizer.depthBiasEnable = pDesc->pRasterizerState->depthBiasEnable;
        rasterizer.depthBiasConstantFactor = pDesc->pRasterizerState->depthBiasConstantFactor;
        rasterizer.depthBiasClamp = pDesc->pRasterizerState->depthBiasClamp;
        rasterizer.depthBiasSlopeFactor = pDesc->pRasterizerState->depthBiasSlopeFactor;
    }
    else
    {
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    }

    // Define multisampling
    VkPipelineMultisampleStateCreateInfo multisampling = {};

    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = textureSamplesToVkSamples(pDesc->sampleCount);
    multisampling.minSampleShading = 0.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    // Define color blending for an attachment.
    VkPipelineColorBlendAttachmentState colorBlendAttachments[8];

    for (uint32_t i = 0; i < pDesc->numRenderTargets; i++)
    {
        if (pDesc->pBlendState != nullptr && pDesc->pBlendState->renderTargetBlendStates[i].blendEnable)
        {
            colorBlendAttachments[i] = {};
            colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachments[i].blendEnable = pDesc->pBlendState->renderTargetBlendStates[i].blendEnable;
            colorBlendAttachments[i].srcColorBlendFactor = gBlendFactorTranslator[pDesc->pBlendState->renderTargetBlendStates[i].srcColorBlendFactor];
            colorBlendAttachments[i].dstColorBlendFactor = gBlendFactorTranslator[pDesc->pBlendState->renderTargetBlendStates[i].dstColorBlendFactor];
            colorBlendAttachments[i].colorBlendOp = gBlendOpTranslator[pDesc->pBlendState->renderTargetBlendStates[i].colorBlendOp];
            colorBlendAttachments[i].srcAlphaBlendFactor = gBlendFactorTranslator[pDesc->pBlendState->renderTargetBlendStates[i].srcAlphaBlendFactor];
            colorBlendAttachments[i].dstAlphaBlendFactor = gBlendFactorTranslator[pDesc->pBlendState->renderTargetBlendStates[i].dstAlphaBlendFactor];
            colorBlendAttachments[i].alphaBlendOp = gBlendOpTranslator[pDesc->pBlendState->renderTargetBlendStates[i].alphaBlendOp];
        }
        else
        {
            colorBlendAttachments[i] = {};
            colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachments[i].blendEnable = VK_FALSE;
        }
    }


    // Define pipeline color blending.
    VkPipelineColorBlendStateCreateInfo colorBlending = {};

    if (pDesc->pBlendState != nullptr)
    {
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = pDesc->pBlendState->logicOpEnable;
        colorBlending.logicOp = gLogicOpTranslator[pDesc->pBlendState->logicOp];
        colorBlending.attachmentCount = pDesc->numRenderTargets;
        colorBlending.pAttachments = &colorBlendAttachments[0];
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional
    }
    else
    {
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = pDesc->numRenderTargets;
        colorBlending.pAttachments = &colorBlendAttachments[0];
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional
    }

    // Define depth stencil state.
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};

    if (pDesc->pDepthStencilState != nullptr)
    {
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = pDesc->pDepthStencilState->depthTestEnable;
        depthStencil.depthWriteEnable = pDesc->pDepthStencilState->depthWriteEnable;
        depthStencil.depthCompareOp = gCompareOpTranslator[pDesc->pDepthStencilState->depthCompareOp];
        depthStencil.depthBoundsTestEnable = pDesc->pDepthStencilState->depthBoundsTestEnable;
        depthStencil.minDepthBounds = pDesc->pDepthStencilState->minDepthBounds;
        depthStencil.maxDepthBounds = pDesc->pDepthStencilState->maxDepthBounds;
        depthStencil.stencilTestEnable = pDesc->pDepthStencilState->stencilTestEnable;

        depthStencil.front.failOp = gStencilOpTranslator[pDesc->pDepthStencilState->front.failOp];
        depthStencil.front.passOp = gStencilOpTranslator[pDesc->pDepthStencilState->front.passOp];
        depthStencil.front.depthFailOp = gStencilOpTranslator[pDesc->pDepthStencilState->front.depthFailOp];
        depthStencil.front.compareOp = gCompareOpTranslator[pDesc->pDepthStencilState->front.compareOp];
        depthStencil.front.compareMask = pDesc->pDepthStencilState->front.compareMask;
        depthStencil.front.writeMask = pDesc->pDepthStencilState->front.writeMask;
        depthStencil.front.reference = 0;

        depthStencil.back.failOp = gStencilOpTranslator[pDesc->pDepthStencilState->back.failOp];
        depthStencil.back.passOp = gStencilOpTranslator[pDesc->pDepthStencilState->back.passOp];
        depthStencil.back.depthFailOp = gStencilOpTranslator[pDesc->pDepthStencilState->back.depthFailOp];
        depthStencil.back.compareOp = gCompareOpTranslator[pDesc->pDepthStencilState->back.compareOp];
        depthStencil.back.compareMask = pDesc->pDepthStencilState->back.compareMask;
        depthStencil.back.writeMask = pDesc->pDepthStencilState->back.writeMask;
        depthStencil.back.reference = 0;

    }
    else
    {
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional
    }

    // Combine scissor rects and viewport into a viewport state.
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    // Define dynamic states that we can change per draw.
    VkDynamicState dynamicStates[] =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
        VK_DYNAMIC_STATE_DEPTH_BIAS,
        VK_DYNAMIC_STATE_DEPTH_BOUNDS,
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 5;
    dynamicState.pDynamicStates = dynamicStates;

    Vector<VkPushConstantRange> pushConstants(pDesc->numPushConstantRanges);

    for (uint32_t i = 0; i < pDesc->numPushConstantRanges; ++i)
    {
        pushConstants[i].stageFlags = shaderStageFlagsToVkShaderStageFlags(pDesc->pPushConstantRanges[i].stageFlags);
        pushConstants[i].offset = pDesc->pPushConstantRanges[i].offset;
        pushConstants[i].size = pDesc->pPushConstantRanges[i].size;
    }

    // Create pipeline layout.
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = pDesc->numPushConstantRanges;
    pipelineLayoutInfo.pPushConstantRanges = pDesc->numPushConstantRanges != 0 ? pushConstants.data() : nullptr;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(rootSignature->descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = rootSignature->descriptorSetLayouts.data();

    if (vkCreatePipelineLayout(_device->device, &pipelineLayoutInfo, nullptr, &pipeline->layout) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create pipeline layout!");
        delete pipeline;
        return RR_INITIALIZATION_FAILED;
    }

    VulkanRenderPass* pass;
    VulkanRenderPassCreateDesc passDesc;
    passDesc.numColorAttachments = pDesc->numRenderTargets;
    memcpy(&passDesc.colorAttachmentFormats, pDesc->renderTargetFormats, sizeof(passDesc.colorAttachmentFormats));
    passDesc.depthStencilAttachmentFormat = pDesc->depthStencilFormat;
    passDesc.sampleCount = pDesc->sampleCount;
    passDesc.sampleQuality = pDesc->sampleQuality;
    passDesc.pLoadStoreOps = nullptr;

    // Create temporary render pass for this pipeline.
    if (create_render_pass(&pass, &passDesc) != RR_SUCCESS)
    {
        RLOG_ERROR("Failed to create render pass.");
        vkDestroyPipelineLayout(_device->device, pipeline->layout, nullptr);

        delete pipeline;
        return RR_INITIALIZATION_FAILED;
    }

    // Create graphics pipeline.
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = numShaderStages;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipeline->layout;
    pipelineInfo.renderPass = pass->pass;
    pipelineInfo.subpass = 0;

    if (vkCreateGraphicsPipelines(_device->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->pipeline) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create graphics pipeline!");
        vkDestroyPipelineLayout(_device->device, pipeline->layout, nullptr);
        destroy_render_pass(pass);

        delete pipeline;
        return RR_INITIALIZATION_FAILED;
    }

    // We don't need the temporary render pass anymore.
    destroy_render_pass(pass);

    pipeline->rootSignature = rootSignature;
    *ppPipeline = (IPipeline*)pipeline;

    return RR_SUCCESS;
}


RenderResult VulkanRenderer::create_pipeline(IPipeline** ppPipeline, ComputePipelineCreateDesc* pDesc)
{
    RASSERT(pDesc != nullptr);
    RASSERT(_initialized);

    RASSERT(pDesc->pShader != nullptr);
    RASSERT(pDesc->pRootSignature != nullptr);

    VulkanPipeline* pipeline = new VulkanPipeline();
    pipeline->type = PIPELINE_TYPE_COMPUTE;

    // Create pipeline shader stage.
    VkPipelineShaderStageCreateInfo shaderStageInfo = {};

    VulkanShader* shader = static_cast<VulkanShader*>(pDesc->pShader);
    VulkanRootSignature* rootSignature = static_cast<VulkanRootSignature*>(pDesc->pRootSignature);

    if (shader->shaders[ShaderStage::SHADER_STAGE_COMPUTE] != VK_NULL_HANDLE)
    {
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStageInfo.module = shader->shaders[ShaderStage::SHADER_STAGE_COMPUTE];
        shaderStageInfo.pName = "main";
    }
    else
    {
        RLOG_ERROR("We are trying to create a compute pipeline while the associated shader program does not include a compute shader.");
        delete pipeline;
        return RR_INITIALIZATION_FAILED;
    }

    // Create pipeline layout.
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = 0; // Optional
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(rootSignature->descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = rootSignature->descriptorSetLayouts.data();

    if (vkCreatePipelineLayout(_device->device, &pipelineLayoutInfo, nullptr, &pipeline->layout) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create pipeline layout!");
        delete pipeline;
        return RR_INITIALIZATION_FAILED;
    }

    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.flags = 0;
    pipelineInfo.layout = pipeline->layout;
    pipelineInfo.stage = shaderStageInfo;

    if (vkCreateComputePipelines(_device->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->pipeline) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create graphics pipeline!");
        vkDestroyPipelineLayout(_device->device, pipeline->layout, nullptr);

        delete pipeline;
        return RR_INITIALIZATION_FAILED;
    }

    // Assign root signature pointer to this pipeline.
    pipeline->rootSignature = rootSignature;

    *ppPipeline = (IPipeline*)pipeline;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_pipeline(IPipeline* pPipeline)
{
    RASSERT(pPipeline != VK_NULL_HANDLE);

    VulkanPipeline* pipeline = static_cast<VulkanPipeline*>(pPipeline);
    RASSERT(pipeline->layout != VK_NULL_HANDLE);
    RASSERT(pipeline->pipeline != VK_NULL_HANDLE);

    vkDestroyPipeline(_device->device, pipeline->pipeline, nullptr);
    vkDestroyPipelineLayout(_device->device, pipeline->layout, nullptr);

    delete pPipeline;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_fence(IFence** ppFence, uint64_t value)
{
    RASSERT(_initialized);

    VulkanFence* fence = new VulkanFence();

    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = 0; // We have no need to initialize a fence in a signaled state.

    // Create fence object.
    if (vkCreateFence(_device->device, &createInfo, nullptr, &fence->fence) != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to create a fence object.");
        return RR_INITIALIZATION_FAILED;
        delete fence;
    }

    fence->submitted = false;
    *ppFence = (IFence*)fence;

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_fence(IFence* pFence)
{
    RASSERT(pFence != nullptr);
    VulkanFence* fence = static_cast<VulkanFence*>(pFence);

    // Destroy fence object.
    vkDestroyFence(_device->device, fence->fence, nullptr);

    delete pFence;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_frame_buffer(VulkanFrameBuffer* pFrameBuffer)
{
    RASSERT(pFrameBuffer != nullptr);

    // Destroy fence object.
    vkDestroyFramebuffer(_device->device, pFrameBuffer->frameBuffer, nullptr);

    delete pFrameBuffer;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_semaphore(ISemaphore** ppSemaphore)
{
    RASSERT(_initialized);

    VulkanSemaphore* semaphore = new VulkanSemaphore();

    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.flags = 0; // We have no need to initialize a semaphore in a signaled state.

    // Create semaphore object.
    if (vkCreateSemaphore(_device->device, &createInfo, nullptr, &semaphore->semaphore) != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to create a semaphore object.");
        return RR_INITIALIZATION_FAILED;
        delete semaphore;
    }

    *ppSemaphore = (ISemaphore*)semaphore;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_semaphore(ISemaphore* pSemaphore)
{
    RASSERT(pSemaphore != nullptr);
    VulkanSemaphore* semaphore = static_cast<VulkanSemaphore*>(pSemaphore);

    // Destroy semaphore object.
    vkDestroySemaphore(_device->device, semaphore->semaphore, nullptr);

    delete pSemaphore;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_render_pass(VulkanRenderPass** ppRenderPass, VulkanRenderPassCreateDesc* pDesc)
{
    RASSERT(pDesc != nullptr);
    RASSERT(_initialized);

    VulkanRenderPass* pass = new VulkanRenderPass();
    pass->desc = *pDesc;

    Vector<VkAttachmentReference> colorAttachmentReferences(pDesc->numColorAttachments);
    Vector<VkAttachmentDescription> attachments(pDesc->numColorAttachments);

    uint32_t numAttachments = 0;

    for (uint32_t i = 0; i < pDesc->numColorAttachments; ++i)
    {
        if (pDesc->colorAttachmentFormats[i] != TEXTURE_FORMAT_UNKNOWN)
        {
            attachments[numAttachments] = {};
            attachments[numAttachments].format = gTextureFormatTranslator[pDesc->colorAttachmentFormats[i]];
            attachments[numAttachments].samples = textureSamplesToVkSamples(pDesc->sampleCount);

            if (pDesc->pLoadStoreOps != nullptr)
            {
                attachments[numAttachments].loadOp = gLoadOpTranslator[pDesc->pLoadStoreOps->colorLoadOps[i]];
                attachments[numAttachments].storeOp = gStoreOpTranslator[pDesc->pLoadStoreOps->colorStoreOps[i]];
            }
            else
            {
                attachments[numAttachments].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                attachments[numAttachments].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            }

            attachments[numAttachments].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments[numAttachments].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            // define color attachment reference.
            colorAttachmentReferences[numAttachments] = {};
            colorAttachmentReferences[numAttachments].attachment = numAttachments; // we want the first color attachment.
            colorAttachmentReferences[numAttachments].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            // Increment amount of attachments.
            ++numAttachments;
        }
    }

    VkAttachmentDescription depthAttachment = {};
    VkAttachmentReference depthAttachmentRef = {};

    if (pDesc->depthStencilAttachmentFormat != TEXTURE_FORMAT_UNKNOWN)
    {
        // Create depth attachment
        depthAttachment.format = gTextureFormatTranslator[pDesc->depthStencilAttachmentFormat];
        depthAttachment.samples = textureSamplesToVkSamples(pDesc->sampleCount);

        if (pDesc->pLoadStoreOps != nullptr)
        {
            depthAttachment.loadOp = gLoadOpTranslator[pDesc->pLoadStoreOps->depthLoadOp];
            depthAttachment.storeOp = gStoreOpTranslator[pDesc->pLoadStoreOps->depthStoreOp];
            depthAttachment.stencilLoadOp = gLoadOpTranslator[pDesc->pLoadStoreOps->stencilLoadOp];
            depthAttachment.stencilStoreOp = gStoreOpTranslator[pDesc->pLoadStoreOps->stencilStoreOp];
        }
        else
        {
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        }

        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // define depth attachment reference.
        depthAttachmentRef.attachment = pDesc->numColorAttachments;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        attachments.push_back(depthAttachment);
    }

    // Describe a subpass.
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = pDesc->colorAttachmentFormats[0] != TEXTURE_FORMAT_UNKNOWN ? numAttachments : 0;
    subpass.pColorAttachments = pDesc->colorAttachmentFormats[0] != TEXTURE_FORMAT_UNKNOWN ? colorAttachmentReferences.data() : nullptr;
    subpass.pDepthStencilAttachment = pDesc->depthStencilAttachmentFormat != TEXTURE_FORMAT_UNKNOWN ? &depthAttachmentRef : nullptr;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 0;
    renderPassInfo.pDependencies = nullptr;

    // Create a render pass using the specified attachments.
    if (vkCreateRenderPass(_device->device, &renderPassInfo, nullptr, &pass->pass) != VK_SUCCESS)
    {
        delete pass;
        RLOG_ERROR("failed to create render pass!");
        return RR_INITIALIZATION_FAILED;
    }

    *ppRenderPass = pass;
    return RR_SUCCESS;
}

void VulkanRenderer::destroy_render_pass(VulkanRenderPass* pRenderPass)
{
    RASSERT(pRenderPass != nullptr);

    vkDestroyRenderPass(_device->device, pRenderPass->pass, nullptr);
    delete pRenderPass;
}

RenderResult VulkanRenderer::create_root_signature(IRootSignature** ppRootSignature, RootSignatureCreateDesc* pDesc)
{
    RASSERT(_initialized);
    RASSERT(_device != nullptr);

    VulkanRootSignature* rootSignature = new VulkanRootSignature();

    uint32_t numSets = 0;
    for (uint32_t i = 0; i < pDesc->numDescriptorBindings; ++i)
    {
        if (numSets < pDesc->pDescriptorBindings[i].set)
        {
            numSets = pDesc->pDescriptorBindings[i].set;
        }
    }

    // Increment numSets since the current value is the highest set number.
    ++numSets;

    rootSignature->descriptorSetLayouts.resize(numSets);

    for (uint32_t set = 0; set < numSets; ++set)
    {
        // Create vector of descriptor set bindings.
        Vector<VkDescriptorSetLayoutBinding> descriptorSetBindings;

        // Create descriptor set binding layout.
        for (uint32_t i = 0; i < pDesc->numDescriptorBindings; ++i)
        {
            if (pDesc->pDescriptorBindings[i].set != set)
            {
                // Skip this descriptor.
                continue;
            }

            DescriptorLayoutBinding& descBinding = pDesc->pDescriptorBindings[i];

            VkDescriptorSetLayoutBinding binding;
            binding.binding = descBinding.binding;
            binding.descriptorType = gDescriptorTypeTranslator[descBinding.descriptorType];
            binding.descriptorCount = descBinding.descriptorCount;
            binding.stageFlags = shaderStageFlagsToVkShaderStageFlags(descBinding.stageFlags);

            // Create array of VkSampler objects to bind.
            if (descBinding.descriptorType == DESCRIPTOR_TYPE_SAMPLER)
            {
                VulkanSampler* sampler = static_cast<VulkanSampler*>(descBinding.ppImmutableSamplers[0]);
                RASSERT(sampler != nullptr);

                binding.pImmutableSamplers = &sampler->sampler;
            }
            else
            {
                binding.pImmutableSamplers = nullptr;
            }

            descriptorSetBindings.push_back(binding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetBindings.size());
        layoutInfo.pBindings = descriptorSetBindings.data();

        // Create vulkan descriptor set layout.
        if (vkCreateDescriptorSetLayout(_device->device, &layoutInfo, nullptr, &rootSignature->descriptorSetLayouts[set]) != VK_SUCCESS)
        {
            RLOG_ERROR("failed to create descriptor set layout!");
            delete rootSignature;
            return RR_INITIALIZATION_FAILED;
        }
    }

    rootSignature->inputAttributeDescriptions.resize(pDesc->pInputAssemblerLayout->numInputAttributeDescriptions);
    rootSignature->inputBindingDescriptions.resize(pDesc->pInputAssemblerLayout->numInputBindingDescriptions);

    // Create input attribute descriptions.
    for (uint32_t i = 0; i < pDesc->pInputAssemblerLayout->numInputAttributeDescriptions; ++i)
    {
        InputAttributeDescription* attribute = &pDesc->pInputAssemblerLayout->pInputAttributeDescriptions[i];
        rootSignature->inputAttributeDescriptions[i].binding = attribute->binding;
        rootSignature->inputAttributeDescriptions[i].format = gTextureFormatTranslator[attribute->format];
        rootSignature->inputAttributeDescriptions[i].location = attribute->location;
        rootSignature->inputAttributeDescriptions[i].offset = attribute->offset;
    }

    // Create input binding descriptions.
    for (uint32_t i = 0; i < pDesc->pInputAssemblerLayout->numInputBindingDescriptions; ++i)
    {
        InputBindingDescription* binding = &pDesc->pInputAssemblerLayout->pInputBindingDescriptions[i];
        rootSignature->inputBindingDescriptions[i].binding = binding->binding;
        rootSignature->inputBindingDescriptions[i].inputRate = gInputRateTranslator[binding->inputRate];
        rootSignature->inputBindingDescriptions[i].stride = binding->stride;
    }

    // Assign shader to this root signature object.
    rootSignature->pShader = static_cast<VulkanShader*>(pDesc->pShaderProgram);

    *ppRootSignature = static_cast<IRootSignature*>(rootSignature);
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_root_signature(IRootSignature* pRootSignature)
{
    RASSERT(pRootSignature != nullptr);

    VulkanRootSignature* rootSignature = static_cast<VulkanRootSignature*>(pRootSignature);

    for (uint32_t i = 0; i < rootSignature->descriptorSetLayouts.size(); ++i)
    {
        vkDestroyDescriptorSetLayout(_device->device, rootSignature->descriptorSetLayouts[i], nullptr);
    }

    delete pRootSignature;

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_shader(IShader** ppShader, ShaderCreateDesc* pDesc)
{
    RASSERT(_initialized);
    RASSERT(_device != nullptr);

    VulkanShader* shader = new VulkanShader();
    memset(shader->shaders, 0, sizeof(shader->shaders));

    for (uint32_t i = 0; i < MAX_SHADER_STAGE; ++i)
    {
        ShaderProperty& shaderProperty = pDesc->shaders[i];

        if (shaderProperty.shaderCodeSize != 0)
        {
            if (shader->shaders[shaderProperty.stage] != VK_NULL_HANDLE)
            {
                RLOG_ERROR("Trying to create a shader program which has multiple shaders for the same stage is incorrect.");
                return RR_ALREADY_INITIALIZED;
            }

            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = shaderProperty.shaderCodeSize;
            createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderProperty.shaderCode);

            if (vkCreateShaderModule(_device->device, &createInfo, nullptr, &shader->shaders[pDesc->shaders[i].stage]) != VK_SUCCESS)
            {
                delete shader;
                RLOG_ERROR("failed to create shader module!");

                return RR_INITIALIZATION_FAILED;
            }
        }
    }

    *ppShader = (IShader*)shader;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_shader(IShader* pShader)
{
    RASSERT(_initialized);
    RASSERT(_device != nullptr);
    RASSERT(pShader != nullptr);

    VulkanShader* shader = static_cast<VulkanShader*>(pShader);

    for (uint32_t i = 0; i < MAX_SHADER_STAGE; ++i)
    {
        if (shader->shaders[i] != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(_device->device, shader->shaders[i], nullptr);
        }
    }

    delete shader;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_sampler(ISampler** ppSampler, SamplerCreateDesc* pDesc)
{
    RASSERT(_initialized);
    RASSERT(_device != nullptr);
    RASSERT(pDesc != nullptr);

    VulkanSampler* sampler = new VulkanSampler();

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.addressModeU = gSamplerAddressModeTranslator[pDesc->addressModeU];
    samplerInfo.addressModeV = gSamplerAddressModeTranslator[pDesc->addressModeV];
    samplerInfo.addressModeW = gSamplerAddressModeTranslator[pDesc->addressModeW];
    samplerInfo.anisotropyEnable = pDesc->anisotropyEnable;
    samplerInfo.borderColor = gBorderColorTranslator[pDesc->borderColor];
    samplerInfo.compareEnable = pDesc->comparisonFunc != SamplerComparisonFunc::SAMPLER_COMPARISON_NONE;
    samplerInfo.compareOp = gSamplerComparisonFuncTranslator[pDesc->comparisonFunc];
    samplerInfo.flags = 0;
    samplerInfo.magFilter = gSamplerFilterTranslator[pDesc->magFilter];
    samplerInfo.minFilter = gSamplerFilterTranslator[pDesc->minFilter];
    samplerInfo.maxAnisotropy = pDesc->maxAnisotropy;
    samplerInfo.maxLod = pDesc->maxLod;
    samplerInfo.minLod = pDesc->minLod;
    samplerInfo.mipLodBias = pDesc->mipLodBias;
    samplerInfo.mipmapMode = gMipModeTranslator[pDesc->mipmapMode];
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.pNext = nullptr;

    // Create image object.
    if (vkCreateSampler(_device->device, &samplerInfo, nullptr, &sampler->sampler) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create sampler!");
        delete sampler;
        return RR_INITIALIZATION_FAILED;
    }

    // Assign sampler description.
    sampler->desc = *pDesc;

    // Set the image description of the image to the one of the created image.
    *ppSampler = sampler;

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_sampler(ISampler* pSampler)
{
    RASSERT(pSampler != nullptr);

    VulkanSampler* sampler = static_cast<VulkanSampler*>(pSampler);
    vkDestroySampler(_device->device, sampler->sampler, nullptr);

    delete pSampler;
    return RR_SUCCESS;
}

uint32_t VulkanRenderer::find_suitable_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_device->physicalDevice->physicalDevice, &memProperties);

    // Find a suitable memory type which has the properties we require.
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    RLOG_ERROR("Failed to find a suitable memory type using the specified flags.");

    return UINT32_MAX;
}

RenderResult VulkanRenderer::create_image(IImage** ppTexture, ImageCreateDesc* pDesc)
{
    RASSERT(pDesc != nullptr);
    RASSERT(pDesc->arrayLayers > 0);

    VulkanImage* image = new VulkanImage();

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = gImageTypeTranslator[pDesc->type];
    imageInfo.extent.width = pDesc->width;
    imageInfo.extent.height = pDesc->height;
    imageInfo.extent.depth = pDesc->depth;
    imageInfo.mipLevels = pDesc->mipLevels == 0 ? (static_cast<uint32_t>(std::floor(std::log2(std::max(pDesc->width, pDesc->height)))) + 1) : pDesc->mipLevels;
    imageInfo.arrayLayers = pDesc->type == IMAGE_TYPE_CUBE ? pDesc->arrayLayers * 6 : pDesc->arrayLayers;
    imageInfo.format = gTextureFormatTranslator[pDesc->format];
    imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = textureUsageFlagsToVkImageFlags(pDesc->usage) | (pDesc->mipLevels == 0 ? (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT) : (VkImageUsageFlags)0);
    imageInfo.samples = textureSamplesToVkSamples(pDesc->sampleCount);
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (pDesc->nativeSwapchainImage == nullptr)
    {
        // Create image object.
        if (vkCreateImage(_device->device, &imageInfo, nullptr, &image->image) != VK_SUCCESS)
        {
            RLOG_ERROR("failed to create image!");
            delete image;
            return RR_INITIALIZATION_FAILED;
        }
    }
    else
    {
        image->image = (VkImage)pDesc->nativeSwapchainImage;
    }


    // Not thread-safe.
    image->textureId = ++gMaxTextureId;

    // Set the image description of the image to the one of the created image.
    image->desc = *pDesc;
    image->desc.mipLevels = image->desc.mipLevels == 0 ? ((uint32_t)std::floor(std::log2(std::max(pDesc->width, pDesc->height))) + 1) : pDesc->mipLevels;

    image->vkImageDesc = imageInfo;
    image->state = ResourceState::RESOURCE_STATE_UNDEFINED;

    if (pDesc->nativeSwapchainImage == nullptr)
    {
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = gMemoryTypeTranslator[pDesc->memoryType];

        vmaAllocateMemoryForImage(_allocator, image->image, &allocInfo, &image->allocation, nullptr);
        vmaBindImageMemory(_allocator, image->allocation, image->image);
    }

    VkImageViewType viewType;

    switch (pDesc->type)
    {
    case IMAGE_TYPE_1D:
    {
        viewType = pDesc->arrayLayers <= 1 ? VK_IMAGE_VIEW_TYPE_1D : VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    }
    break;
    case IMAGE_TYPE_2D:
    {
        viewType = pDesc->arrayLayers <= 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }
    break;
    case IMAGE_TYPE_3D:
    {
        RASSERT(pDesc->arrayLayers == 1);
        viewType = VK_IMAGE_VIEW_TYPE_3D;
    }
    break;
    case IMAGE_TYPE_CUBE:
    {
        RASSERT(pDesc->arrayLayers >= 6);
        RASSERT(pDesc->arrayLayers % 6 == 0);

        viewType = pDesc->arrayLayers == 6 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    }
    break;
    default:
    {
        viewType = VK_IMAGE_VIEW_TYPE_2D;
        RLOG_ERROR("We received an unknown image type while creating a image.");
    }
    break;
    }

    // Create image view.
    image->imageView = create_image_view(image, viewType, imageInfo.format, imageAspectFlagFromFormat(imageInfo.format), image->vkImageViewDesc);

    // Copy initial data inside the buffer.
    if (image->desc.pData != nullptr)
    {
        //create a staging buffer.
        VulkanBuffer* stagingBuffer = nullptr;

        BufferCreateDesc stagingCreateDesc = {};
        stagingCreateDesc.memoryType = MEMORY_TYPE_CPU;
        stagingCreateDesc.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;
        stagingCreateDesc.pData = nullptr;
        stagingCreateDesc.size = (uint32_t)image->allocation->GetSize();

        if (create_buffer((IBuffer **)& stagingBuffer, &stagingCreateDesc) != RR_SUCCESS)
        {
            RLOG_WARNING("Failed to allocate a staging buffer, therefore the newly created buffer won't have any contents.");
        }

        // Copy data into staging buffer.
        void* data;
        vmaMapMemory(_allocator, stagingBuffer->allocation, &data);
        memcpy(data, pDesc->pData, pDesc->dataSize != 0 ? pDesc->dataSize : image->allocation->GetSize());
        vmaUnmapMemory(_allocator, stagingBuffer->allocation);

        VulkanFence* pTransferCompleteFence = nullptr;
        VulkanCommandList* pTransferCmd = nullptr;
        create_command_list((ICommandList**)& pTransferCmd, _transfer_command_pool);
        create_fence((IFence**)& pTransferCompleteFence);

        // copy buffer.
        cmd_begin(pTransferCmd);
        cmd_copy_buffer_to_image(pTransferCmd, stagingBuffer, image, 0, pDesc->width, pDesc->height);
        cmd_end(pTransferCmd);

        // Submit command 
        queue_submit(_transfer_queue, 1, (ICommandList **)& pTransferCmd, pTransferCompleteFence, 0, nullptr, 0, nullptr);

        // Wait for transfer to complete.
        wait_for_fence(pTransferCompleteFence);
        destroy_fence(pTransferCompleteFence);
        destroy_command_list(_transfer_command_pool, pTransferCmd);
        destroy_buffer(stagingBuffer);
    }

    // Generate mip-maps
    if (pDesc->mipLevels == 0)
    {

        VulkanFence* pMipGenCompleteFence = nullptr;
        VulkanCommandList* pMipCmd = nullptr;
        create_command_list((ICommandList**)& pMipCmd, _graphics_command_pool);
        create_fence((IFence**)& pMipGenCompleteFence);

        // copy buffer.
        cmd_begin(pMipCmd);

        ImageBarrier tBarrier;
        tBarrier.image = image;
        tBarrier.requiredState = ResourceState::RESOURCE_STATE_COPY_DEST;

        cmd_resource_barriers(pMipCmd, 0, nullptr, 1, &tBarrier, true);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image->image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = pDesc->width;
        int32_t mipHeight = pDesc->height;

        for (uint32_t mip = 1; mip < image->desc.mipLevels; ++mip)
        {
            barrier.subresourceRange.baseMipLevel = mip - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(pMipCmd->cmdBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit = {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = mip - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { std::max(mipWidth / 2, 1), std::max(mipHeight / 2, 1), 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = mip;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(pMipCmd->cmdBuffer,
                image->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(pMipCmd->cmdBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = image->desc.mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(pMipCmd->cmdBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        cmd_end(pMipCmd);

        // Submit command 
        queue_submit(_graphics_queue, 1, (ICommandList**)& pMipCmd, pMipGenCompleteFence, 0, nullptr, 0, nullptr);

        // Wait for transfer to complete.
        wait_for_fence(pMipGenCompleteFence);
        destroy_fence(pMipGenCompleteFence);
        destroy_command_list(_graphics_command_pool, pMipCmd);
    }

    *ppTexture = image;

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_image(IImage* pTexture)
{
    RASSERT(pTexture != nullptr);

    VulkanImage* image = static_cast<VulkanImage*>(pTexture);

    vkDestroyImageView(_device->device, image->imageView, nullptr);
    vkDestroyImage(_device->device, image->image, nullptr);
    vmaFreeMemory(_allocator, image->allocation);

    delete pTexture;

    return RR_SUCCESS;
}


RenderResult VulkanRenderer::create_buffer(IBuffer** ppBuffer, BufferCreateDesc* pDesc)
{
    RASSERT(pDesc != nullptr);

    VulkanBuffer* buffer = new VulkanBuffer();

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = pDesc->size;
    bufferInfo.usage = bufferUsageFlagsToVkBufferFlags(pDesc->usage);
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = gMemoryTypeTranslator[pDesc->memoryType];

    // Create image object.
    if (vmaCreateBuffer(_allocator, &bufferInfo, &allocInfo, &buffer->buffer, &buffer->allocation, nullptr) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create buffer!");
        delete buffer;
        return RR_INITIALIZATION_FAILED;
    }

    // Set the image description of the image to the one of the created image.
    buffer->desc = *pDesc;
    buffer->state = ResourceState::RESOURCE_STATE_COMMON;

    // Copy initial data inside the buffer.
    if (buffer->desc.pData != nullptr)
    {
        // If we have coherent memory, copy directly into the memory.
        if (buffer->desc.memoryType == MemoryType::MEMORY_TYPE_CPU)
        {
            void* data;
            VkResult res = vmaMapMemory(_allocator, buffer->allocation, &data);
            memcpy(data, pDesc->pData, pDesc->dataSize != 0 ? pDesc->dataSize : pDesc->size);
            vmaUnmapMemory(_allocator, buffer->allocation);
        }
        else
        {
            //create a staging buffer.
            VulkanBuffer* stagingBuffer = nullptr;

            BufferCreateDesc stagingCreateDesc = {};
            stagingCreateDesc.memoryType = MEMORY_TYPE_CPU;
            stagingCreateDesc.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;
            stagingCreateDesc.pData = nullptr;
            stagingCreateDesc.size = pDesc->size;

            if (create_buffer((IBuffer**)& stagingBuffer, &stagingCreateDesc) != RR_SUCCESS)
            {
                RLOG_WARNING("Failed to allocate a staging buffer, therefore the newly created buffer won't have any contents.");
            }

            // Copy data into staging buffer.
            void* data;
            vmaMapMemory(_allocator, stagingBuffer->allocation, &data);
            memcpy(data, pDesc->pData, pDesc->dataSize != 0 ? pDesc->dataSize : pDesc->size);
            vmaUnmapMemory(_allocator, stagingBuffer->allocation);

            VulkanFence* pTransferCompleteFence = nullptr;
            VulkanCommandList* pTransferCmd = nullptr;
            create_command_list((ICommandList **)& pTransferCmd, _transfer_command_pool);
            create_fence((IFence **)& pTransferCompleteFence);

            // copy buffer.
            cmd_begin(pTransferCmd);
            cmd_copy_buffer(pTransferCmd, stagingBuffer, buffer, 0, 0, static_cast<uint32_t>(pDesc->size));
            cmd_end(pTransferCmd);

            // Submit command 
            queue_submit(_transfer_queue, 1, (ICommandList * *)& pTransferCmd, pTransferCompleteFence, 0, nullptr, 0, nullptr);

            // Wait for transfer to complete.
            wait_for_fence(pTransferCompleteFence);
            destroy_fence(pTransferCompleteFence);
            destroy_command_list(_transfer_command_pool, pTransferCmd);
            destroy_buffer(stagingBuffer);
        }
    }

    *ppBuffer = (IBuffer*)buffer;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_buffer(IBuffer* pBuffer)
{
    RASSERT(pBuffer != nullptr);

    VulkanBuffer* buffer = static_cast<VulkanBuffer*>(pBuffer);
    vmaDestroyBuffer(_allocator, buffer->buffer, buffer->allocation);

    delete pBuffer;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_copy_buffer(ICommandList* pCommandList, IBuffer* pSrcBuffer, IBuffer* pDstBuffer, uint32_t srcOffset, uint32_t dstOffset, uint32_t size)
{
    RASSERT(pCommandList != nullptr);
    RASSERT(pSrcBuffer != nullptr);
    RASSERT(pDstBuffer != nullptr);

    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);
    VulkanBuffer* srcBuffer = static_cast<VulkanBuffer*>(pSrcBuffer);
    VulkanBuffer* dstBuffer = static_cast<VulkanBuffer*>(pDstBuffer);

    BufferBarrier bBarriers[2];

    bBarriers[0].buffer = pSrcBuffer;
    bBarriers[0].requiredState = ResourceState::RESOURCE_STATE_COPY_SOURCE;

    bBarriers[1].buffer = pDstBuffer;
    bBarriers[1].requiredState = ResourceState::RESOURCE_STATE_COPY_DEST;

    cmd_resource_barriers(pCommandList, 2, &bBarriers[0], 0, nullptr, true);

    // Set copy region.
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = srcOffset; // Optional
    copyRegion.dstOffset = dstOffset; // Optional
    copyRegion.size = size;

    // Copy the buffer.
    vkCmdCopyBuffer(list->cmdBuffer, srcBuffer->buffer, dstBuffer->buffer, 1, &copyRegion);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_copy_buffer_to_image(ICommandList* pCommandList, IBuffer* pSrcBuffer, IImage* pDstImage, uint32_t srcOffset, uint32_t width, uint32_t height)
{
    BufferBarrier bBarrier;
    bBarrier.buffer = pSrcBuffer;
    bBarrier.requiredState = ResourceState::RESOURCE_STATE_COPY_SOURCE;

    ImageBarrier tBarrier;
    tBarrier.image = pDstImage;
    tBarrier.requiredState = ResourceState::RESOURCE_STATE_COPY_DEST;

    cmd_resource_barriers(pCommandList, 1, &bBarrier, 1, &tBarrier, true);

    RASSERT(pCommandList != nullptr);
    RASSERT(pSrcBuffer != nullptr);
    RASSERT(pDstImage != nullptr);

    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);
    VulkanBuffer* srcBuffer = static_cast<VulkanBuffer*>(pSrcBuffer);
    VulkanImage* dstImage = static_cast<VulkanImage*>(pDstImage);

    VkBufferImageCopy region = {};
    region.bufferOffset = srcOffset;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        list->cmdBuffer,
        srcBuffer->buffer,
        dstImage->image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    ImageBarrier tEndBarrier;
    tEndBarrier.image = pDstImage;
    tEndBarrier.requiredState = ResourceState::RESOURCE_STATE_SHADER_RESOURCE;

    cmd_resource_barriers(pCommandList, 0, nullptr, 1, &tEndBarrier, true);
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_update_buffer(ICommandList* pCommandList, IBuffer* pDstBuffer, void* pData, uint32_t dstOffset, uint32_t size)
{
    RASSERT(pData != nullptr);
    RASSERT(pCommandList != nullptr);
    RASSERT(pDstBuffer != nullptr);
    RASSERT(size != 0);

    VulkanBuffer* buffer = static_cast<VulkanBuffer*>(pDstBuffer);
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    if (buffer->desc.memoryType == MEMORY_TYPE_CPU)
    {
        void* data;
        vmaMapMemory(_allocator, buffer->allocation, &data);
        memcpy((char*)data + dstOffset, pData, size);
        vmaUnmapMemory(_allocator, buffer->allocation);

        return RR_SUCCESS;
    }
    else if (buffer->desc.memoryType == MEMORY_TYPE_CPU_TO_GPU)
    {
        void* data;
        vmaMapMemory(_allocator, buffer->allocation, &data);
        memcpy((char*)data + dstOffset, pData, size);
        vmaUnmapMemory(_allocator, buffer->allocation);

        VkMemoryPropertyFlags memFlags;
        vmaGetMemoryTypeProperties(_allocator, buffer->allocation->GetMemoryTypeIndex(), &memFlags);

        if ((memFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
        {
            VkMappedMemoryRange memRange = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
            memRange.memory = buffer->allocation->GetMemory();
            memRange.offset = buffer->allocation->GetOffset() + dstOffset;
            memRange.size = size;

            vkFlushMappedMemoryRanges(_device->device, 1, &memRange);
        }
        return RR_SUCCESS;
    }
    else
    {
        //create a staging buffer.
        VulkanBuffer* stagingBuffer = nullptr;

        BufferCreateDesc stagingCreateDesc = {};
        stagingCreateDesc.memoryType = MEMORY_TYPE_CPU;
        stagingCreateDesc.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;
        stagingCreateDesc.pData = nullptr;
        stagingCreateDesc.size = size;

        if (create_buffer((IBuffer**)& stagingBuffer, &stagingCreateDesc) != RR_SUCCESS)
        {
            RLOG_ERROR("Failed to allocate a staging buffer, therefore the newly created buffer won't have any contents.");
            return RR_FAIL;
        }

        // Copy data into staging buffer.
        void* data;
        vmaMapMemory(_allocator, stagingBuffer->allocation, &data);
        memcpy(data, pData, size);
        vmaUnmapMemory(_allocator, stagingBuffer->allocation);

        // copy buffer.
        cmd_copy_buffer(list, stagingBuffer, buffer, 0, dstOffset, size);
        destroy_buffer(stagingBuffer);

        return RR_SUCCESS;
    }
}

RenderResult VulkanRenderer::cmd_update_image(ICommandList* pCommandList, IImage* pDstTexture, void* pData, uint32_t width, uint32_t height)
{
    RASSERT(pData != nullptr);
    RASSERT(pCommandList != nullptr);
    RASSERT(pDstTexture != nullptr);
    RASSERT(!(width == 0 && height == 0));

    VulkanImage* image = static_cast<VulkanImage*>(pDstTexture);
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    //create a staging buffer.
    VulkanBuffer* stagingBuffer = nullptr;

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device->device, image->image, &memRequirements);

    BufferCreateDesc stagingCreateDesc = {};
    stagingCreateDesc.memoryType = MEMORY_TYPE_CPU;
    stagingCreateDesc.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;
    stagingCreateDesc.pData = nullptr;
    stagingCreateDesc.size = memRequirements.size;

    if (create_buffer((IBuffer**)& stagingBuffer, &stagingCreateDesc) != RR_SUCCESS)
    {
        RLOG_WARNING("Failed to allocate a staging buffer, therefore the newly created buffer won't have any contents.");
    }

    // Copy data into staging buffer.
    void* data;
    vmaMapMemory(_allocator, stagingBuffer->allocation, &data);
    memcpy(data, pData, memRequirements.size);
    vmaUnmapMemory(_allocator, stagingBuffer->allocation);

    // Copy buffer.
    cmd_copy_buffer_to_image(list, stagingBuffer, image, 0, width, height);
    destroy_buffer(stagingBuffer);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_render_target(IRenderTarget** ppRenderTarget, RenderTargetCreateDesc* pDesc)
{
    RASSERT(pDesc != nullptr);

    VulkanRenderTarget* renderTarget = new VulkanRenderTarget();

    ImageUsageFlags usage = IMAGE_USAGE_TRANSFER_SRC | IMAGE_USAGE_TRANSFER_DST | IMAGE_USAGE_SAMPLED;
    if ((pDesc->usage & RenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT) != 0)
    {
        usage |= ImageUsageFlags::IMAGE_USAGE_COLOR_ATTACHMENT;
    }

    if ((pDesc->usage & RenderTargetUsage::RENDER_TARGET_USAGE_DEPTH_STENCIL_ATTACHMENT) != 0)
    {
        usage |= ImageUsageFlags::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT;
    }

    ImageCreateDesc createDesc = {};
    createDesc.arrayLayers = 1;
    createDesc.baseArrayLayer = 0;
    createDesc.baseMipLevel = 0;
    createDesc.depth = 1;
    createDesc.format = pDesc->format;
    createDesc.width = pDesc->width;
    createDesc.height = pDesc->height;
    createDesc.memoryType = pDesc->memoryType;
    createDesc.mipLevels = 1;
    createDesc.sampleCount = pDesc->sampleCount;
    createDesc.sampleQuality = pDesc->sampleQuality;
    createDesc.tiling = pDesc->tiling;
    createDesc.type = ImageType::IMAGE_TYPE_2D;
    createDesc.usage = usage;
    createDesc.nativeSwapchainImage = pDesc->nativeSwapchainImage;
    createDesc.pData = nullptr;

    // Create a image for this render target.
    if (create_image((IImage**)&renderTarget->image, &createDesc) != RR_SUCCESS)
    {
        RLOG_ERROR("Failed to create a image for this render target.");
        return RR_INITIALIZATION_FAILED;
    }

    renderTarget->desc = *pDesc;

    *ppRenderTarget = (VulkanRenderTarget*)renderTarget;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::destroy_render_target(IRenderTarget* pRenderTarget)
{
    RASSERT(pRenderTarget != nullptr);

    VulkanRenderTarget* target = static_cast<VulkanRenderTarget*>(pRenderTarget);
    destroy_image(target->image);

    delete pRenderTarget;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::setup_debug_callback()
{
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT; // only errors and warnings.
    createInfo.pfnCallback = debug_callback;

    // Create debug extension.
    if (create_debug_report_callback_extension(_instance, &createInfo, nullptr, &_debugging_callback) != VK_SUCCESS) {
        return RR_INITIALIZATION_FAILED;
    }

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::query_available_physical_devices()
{
    // Query how many physical devices we have.
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

    // We don't have any gpu's with vulkan support so we can't proceed.
    if (deviceCount == 0) {
        return RR_NO_SUITABLE_DEVICE;
    }

    // Resize the gpus vector to hold all physical devices.
    _gpus.resize(deviceCount);

    // Get the list of the physical devices we have.
    Vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    for (uint32_t i = 0; i < deviceCount; i++)
    {
        _gpus[i] = VulkanPhysicalDevice();

        // Fill our physical device structures.
        _gpus[i].physicalDevice = devices[i];

        // Query device properties.
        vkGetPhysicalDeviceProperties(devices[i], &_gpus[i].properties);

        // Query device memory properties.
        vkGetPhysicalDeviceMemoryProperties(devices[i], &_gpus[i].memoryProperties);

        // Query device features.
        vkGetPhysicalDeviceFeatures(devices[i], &_gpus[i].features);

        // Get amount of available queue families.
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);

        // Get the list of queue families.
        _gpus[i].queueFamilyProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, _gpus[i].queueFamilyProperties.data());

        // query the amount of extensions this physical device supports.
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);

        // Retrieve the extension list.
        _gpus[i].supportedExtensions.resize(extensionCount);
        vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, _gpus[i].supportedExtensions.data());
    }

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::pick_physical_device(VulkanPhysicalDevice** ppDevice)
{
    // Safety check to make sure we have retrieved all information about the physical devices.
    if (_gpus.empty())
    {
        RenderResult result = query_available_physical_devices();
        if (result != RR_SUCCESS)
        {
            return result;
        }
    }

    // We require swapchain support for our device.
    Vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    for (auto extension : _renderer_info.requiredVulkanDeviceExtensions)
    {
        deviceExtensions.push_back(extension.c_str());
    }

    // Find the most suitable physical device.
    VulkanPhysicalDevice* mostSuitableDevice = nullptr;
    uint32_t deviceScore = 0;

    for (auto& candidate : _gpus)
    {
        // Calculate score for this device.
        uint32_t score = calculate_physical_device_score(&candidate, deviceExtensions);

        // If this device is suitable, and better than the current candidate, make this device our best candidate.
        if (score != 0 && score > deviceScore)
        {
            mostSuitableDevice = &candidate;
            deviceScore = score;
        }
    }

    // If we dont have any suitable device, return an error code.
    if (mostSuitableDevice == VK_NULL_HANDLE) {
        return RR_NO_SUITABLE_DEVICE;
    }

    // Output our most suitable device to ppDevice.
    *ppDevice = mostSuitableDevice;

    return RR_SUCCESS;
}

QueueFamilyIndices VulkanRenderer::retrieve_queue_family_indices(const VulkanPhysicalDevice* pDevice)
{
    QueueFamilyIndices indices;
    indices.graphicsQueueIndex = retrieve_queue_family_index(pDevice, VK_QUEUE_GRAPHICS_BIT);
    indices.computeQueueIndex = retrieve_queue_family_index(pDevice, VK_QUEUE_COMPUTE_BIT);
    indices.transferQueueIndex = retrieve_queue_family_index(pDevice, VK_QUEUE_TRANSFER_BIT);

    return indices;
}

uint32_t VulkanRenderer::retrieve_queue_family_index(const VulkanPhysicalDevice* pDevice, VkQueueFlags flags)
{
    // Try to find a dedicated compute queue.
    if (flags & VK_QUEUE_COMPUTE_BIT)
    {
        for (uint32_t i = 0; i < pDevice->queueFamilyProperties.size(); i++)
        {
            // If the queue supports compute and does NOT support graphics, we can return this as a dedicated compute queue.
            if (pDevice->queueFamilyProperties[i].queueCount > 0 &&
                (pDevice->queueFamilyProperties[i].queueFlags & flags)
                && ((pDevice->queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                return i;
            }
        }
    }

    // Try to find a dedicated transfer queue.
    if (flags & VK_QUEUE_TRANSFER_BIT)
    {
        for (uint32_t i = 0; i < pDevice->queueFamilyProperties.size(); i++)
        {
            // If the queue supports compute and does NOT support graphics or compute, we can return this as a dedicated compute queue.
            if (pDevice->queueFamilyProperties[i].queueCount > 0 &&
                (pDevice->queueFamilyProperties[i].queueFlags & flags) &&
                ((pDevice->queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((pDevice->queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
            {
                return i;
            }
        }
    }

    // Otherwise return the first queue which matches the flags.
    for (uint32_t i = 0; i < pDevice->queueFamilyProperties.size(); i++)
    {
        // If the queue supports compute and does NOT support graphics or compute, we can return this as a dedicated compute queue.
        if (pDevice->queueFamilyProperties[i].queueCount > 0 &&
            (pDevice->queueFamilyProperties[i].queueFlags & flags)) {
            return i;
        }
    }

    // We couldn't find a queue which supports the specified flags so we return an invalid queue index.
    return UINT32_MAX;
}

uint32_t VulkanRenderer::calculate_physical_device_score(const VulkanPhysicalDevice* pDevice, const Vector<const char*>& rRequiredDeviceExtensions)
{
    uint32_t score = 0;

    // We require device extensions to be available.
    if (!has_device_extension_support(pDevice, rRequiredDeviceExtensions))
    {
        // We don't have required device extensions therefore this device is unsuitable.
        return 0;
    }

    // Retrieve queue family indices.
    QueueFamilyIndices queueFamilyIndices = retrieve_queue_family_indices(pDevice);

    // A dedicated GPU is preferred.
    if (pDevice->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    // If we have dedicated queues for transferral and compute, our score goes up.
    if (queueFamilyIndices.has_dedicated_compute_queue())
    {
        score += 100;
    }

    if (queueFamilyIndices.has_dedicated_transfer_queue())
    {
        score += 100;
    }

    if (pDevice->features.tessellationShader)
    {
        score += 100;
    }

    if (pDevice->features.geometryShader)
    {
        score += 100;
    }

    return score;
}

RenderResult VulkanRenderer::create_logical_device(VulkanPhysicalDevice* pPhysicalDevice)
{
    _device = new VulkanDevice();

    // Find queue indices.
    _device->queueIndices = retrieve_queue_family_indices(pPhysicalDevice);

    // Vector of queue creation information structures.
    Vector<VkDeviceQueueCreateInfo> queueInfos;

    Vector<Vector<float>>queuePriorities(pPhysicalDevice->queueFamilyProperties.size());

    for (uint32_t i = 0; i < pPhysicalDevice->queueFamilyProperties.size(); ++i)
    {
        queuePriorities[i].resize(pPhysicalDevice->queueFamilyProperties[i].queueCount);
        for (uint32_t j = 0; j < pPhysicalDevice->queueFamilyProperties[i].queueCount; ++j)
        {
            queuePriorities[i][j] = 1.0f;
        }

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = i;
        queueCreateInfo.queueCount = pPhysicalDevice->queueFamilyProperties[i].queueCount;
        queueCreateInfo.pQueuePriorities = queuePriorities[i].data();
        queueInfos.push_back(queueCreateInfo);
    }

    // We want certain device features to be enabled.
    _device->enabledFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    createInfo.pEnabledFeatures = &_device->enabledFeatures;

    createInfo.enabledExtensionCount = 0;

    // Enable the same validation layers for this logical device.
    Vector<const char*> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
    };

    for (auto layer : _renderer_info.requiredVulkanDeviceLayers)
    {
        validationLayers.push_back(layer.c_str());
    }

    // Enable swapchain extensions.
    Vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    for (auto extension : _renderer_info.requiredVulkanDeviceExtensions)
    {
        deviceExtensions.push_back(extension.c_str());
    }

    if (_renderer_info.enableDebugging) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Create the device.
    if (vkCreateDevice(pPhysicalDevice->physicalDevice, &createInfo, nullptr, &_device->device) != VK_SUCCESS)
    {
        delete _device;
        _device = nullptr;

        return RR_INITIALIZATION_FAILED;
    }

    _device->physicalDevice = pPhysicalDevice;
    return RR_SUCCESS;
}

bool VulkanRenderer::has_device_extension_support(const VulkanPhysicalDevice* pDevice, const Vector<const char*>& rDeviceExtensions)
{
    // Go over the extensions and remove them if we support it. If the list is empty at the end, it means we support all extensions.
    std::set<std::string> requiredExtensions(rDeviceExtensions.begin(), rDeviceExtensions.end());

    for (const auto& extension : pDevice->supportedExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

VkImageView VulkanRenderer::create_image_view(VulkanImage* pImage, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewCreateInfo& desc)
{
    desc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    desc.image = pImage->image;
    desc.viewType = viewType;
    desc.format = format;
    desc.subresourceRange.aspectMask = aspectFlags;
    desc.subresourceRange.baseMipLevel = pImage->desc.baseMipLevel;
    desc.subresourceRange.levelCount = pImage->desc.mipLevels;
    desc.subresourceRange.baseArrayLayer = pImage->desc.baseArrayLayer;
    desc.subresourceRange.layerCount = pImage->desc.arrayLayers * (pImage->desc.type == IMAGE_TYPE_CUBE ? 6 : 1);
    desc.components.a = VK_COMPONENT_SWIZZLE_A;
    desc.components.r = VK_COMPONENT_SWIZZLE_R;
    desc.components.g = VK_COMPONENT_SWIZZLE_G;
    desc.components.b = VK_COMPONENT_SWIZZLE_B;

    VkImageView imageView;
    if (vkCreateImageView(_device->device, &desc, nullptr, &imageView) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create image image view!");
        return VK_NULL_HANDLE;
    }

    return imageView;
}

RenderResult VulkanRenderer::destroy()
{
    // If the renderer is not initialized, we have nothing to destroy; thefore we immediately return success.
    if (!is_initialized())
    {
        return RR_SUCCESS;
    }

    if (_renderer_info.enableDebugging)
    {
        // Destroy debugging report callback.
        destroy_debug_report_callback_extension(_instance, _debugging_callback, nullptr);
    }

    // Destroy our global descriptor pools.
    for (auto descriptorPool : _descriptor_pools)
    {
        destroy_descriptor_pool(descriptorPool.second);
    }

    _descriptor_pools.clear();

    // Destroy transfer queue and pool.
    destroy_command_pool(_transfer_command_pool);
    destroy_queue(_transfer_queue);

    destroy_command_pool(_graphics_command_pool);
    destroy_queue(_graphics_queue);

    for (auto frameBufferMap : _frame_buffer_maps)
    {
        for (auto it = frameBufferMap.second->hashToBuffer.begin(); it != frameBufferMap.second->hashToBuffer.end(); ++it)
        {
            destroy_frame_buffer(it->second);
        }
    }

    for (auto renderPassMap : _render_pass_maps)
    {
        for (auto it = renderPassMap.second->hashToRenderPass.begin(); it != renderPassMap.second->hashToRenderPass.end(); ++it)
        {
            destroy_render_pass(it->second);
        }
    }

    _frame_buffer_maps.clear();
    _render_pass_maps.clear();

    vmaDestroyAllocator(_allocator);

    // Destroy our logical device.
    vkDestroyDevice(_device->device, nullptr);

    // Destroy dynamically allocated device wrapper.
    delete _device;
    _device = nullptr;

    // Destroy our Vulkan instance.
    vkDestroyInstance(_instance, nullptr);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_descriptor_pool(VulkanDescriptorPool** ppDescriptorPool, VulkanDescriptorPoolCreateDesc* pDesc)
{
    RASSERT(pDesc != nullptr);
    RASSERT(_device != nullptr);

    VulkanDescriptorPool* pool = new VulkanDescriptorPool();

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = pDesc->numPoolSizes;
    poolInfo.pPoolSizes = pDesc->pPoolSizes;
    poolInfo.maxSets = pDesc->maxDescriptorSets;
    poolInfo.flags = pDesc->flags;

    if (vkCreateDescriptorPool(_device->device, &poolInfo, nullptr, &pool->descriptorPool) != VK_SUCCESS)
    {
        RLOG_ERROR("failed to create descriptor pool!");
        delete pool;

        return RR_INITIALIZATION_FAILED;
    }

    pool->flags = pDesc->flags;
    pool->maxDescriptorSets = pDesc->maxDescriptorSets;
    pool->numDescriptorSets = 0;

    *ppDescriptorPool = pool;
    return RR_SUCCESS;
}

void VulkanRenderer::destroy_descriptor_pool(VulkanDescriptorPool* pDescriptorPool)
{
    RASSERT(pDescriptorPool != nullptr);
    vkDestroyDescriptorPool(_device->device, pDescriptorPool->descriptorPool, nullptr);
    delete pDescriptorPool;
}

RenderResult VulkanRenderer::queue_submit(IQueue* pQueue, uint32_t numCmds, ICommandList** ppCmds, IFence* pSignalFence, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores, uint32_t numSignalSemaphores, ISemaphore** ppSignalSemaphores)
{
    RASSERT(pQueue != nullptr);
    RASSERT(numCmds != 0);

    VulkanQueue* queue = static_cast<VulkanQueue*>(pQueue);
    VulkanFence* fence = static_cast<VulkanFence*>(pSignalFence);

    Vector<VkCommandBuffer> commandBuffers(numCmds);
    Vector<VkSemaphore> waitSemaphores(numWaitSemaphores);
    Vector<VkPipelineStageFlags> waitSemaphoreDestMasks(numWaitSemaphores);
    Vector<VkSemaphore> signalSemaphores(numSignalSemaphores);

    // Retrieve vulkan objects from wrapper objects.
    for (uint32_t i = 0; i < numCmds; ++i)
    {
        VulkanCommandList* cmdList = static_cast<VulkanCommandList*>(ppCmds[i]);
        RASSERT(cmdList != nullptr);

        commandBuffers[i] = cmdList->cmdBuffer;
    }

    for (uint32_t i = 0; i < numWaitSemaphores; ++i)
    {
        VulkanSemaphore* semaphore = static_cast<VulkanSemaphore*>(ppWaitSemaphores[i]);
        RASSERT(semaphore != nullptr);

        waitSemaphores[i] = semaphore->semaphore;
        waitSemaphoreDestMasks[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }

    for (uint32_t i = 0; i < numSignalSemaphores; ++i)
    {
        VulkanSemaphore* semaphore = static_cast<VulkanSemaphore*>(ppSignalSemaphores[i]);
        RASSERT(semaphore != nullptr);

        signalSemaphores[i] = semaphore->semaphore;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = numCmds;
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.signalSemaphoreCount = numSignalSemaphores;
    submitInfo.pSignalSemaphores = numSignalSemaphores != 0 ? signalSemaphores.data() : VK_NULL_HANDLE;
    submitInfo.waitSemaphoreCount = numWaitSemaphores;
    submitInfo.pWaitSemaphores = numWaitSemaphores != 0 ? waitSemaphores.data() : VK_NULL_HANDLE;
    submitInfo.pWaitDstStageMask = numWaitSemaphores != 0 ? waitSemaphoreDestMasks.data() : VK_NULL_HANDLE;
    submitInfo.pNext = VK_NULL_HANDLE;

    if (fence != nullptr)
    {
        fence->submitted = true;
    }

    VkResult res = vkQueueSubmit(queue->vkQueue, 1, &submitInfo, fence != nullptr ? fence->fence : VK_NULL_HANDLE);
    // Submit the commands to the queue.
    if (res != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to submit command lists to the specified command queue.");
        return RR_UNKNOWN_FAILURE;
    }

    // We successfully submitted our commands to the queue.
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::wait_for_queue(IQueue* pQueue)
{
    RASSERT(pQueue != nullptr);

    // Get vulkan wrapper object of the queue.
    VulkanQueue* queue = static_cast<VulkanQueue*>(pQueue);

    vkQueueWaitIdle(queue->vkQueue);
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::wait_for_device()
{
    vkDeviceWaitIdle(_device->device);
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::queue_present(IQueue* pQueue, uint32_t numSwapChains, ISwapChain** ppSwapChains, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores)
{
    RASSERT(pQueue != nullptr);
    RASSERT(numSwapChains != 0);

    // Get vulkan wrapper object of the queue.
    VulkanQueue* queue = static_cast<VulkanQueue*>(pQueue);

    // Create array of vulkan semaphore pointers to wait on.
    Vector<VkSemaphore> waitSemaphores(numWaitSemaphores);

    // Create array of vulkan swapchain pointers.
    Vector<VkSwapchainKHR> swapchains(numSwapChains);
    Vector<uint32_t> swapchainImageIndices(numSwapChains);
    Vector<VkResult> results(numSwapChains);

    for (uint32_t i = 0; i < numWaitSemaphores; ++i)
    {
        VulkanSemaphore* semaphore = static_cast<VulkanSemaphore*>(ppWaitSemaphores[i]);
        RASSERT(semaphore != nullptr);

        waitSemaphores[i] = semaphore->semaphore;
    }

    for (uint32_t i = 0; i < numSwapChains; ++i)
    {
        VulkanSwapChain* swapChain = static_cast<VulkanSwapChain*>(ppSwapChains[i]);
        RASSERT(swapChain != nullptr);

        swapchains[i] = swapChain->swapChain;
        swapchainImageIndices[i] = swapChain->imageIndex;
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = VK_NULL_HANDLE;
    presentInfo.swapchainCount = numSwapChains;
    presentInfo.pSwapchains = swapchains.data();
    presentInfo.pImageIndices = swapchainImageIndices.data();
    presentInfo.waitSemaphoreCount = numWaitSemaphores;
    presentInfo.pWaitSemaphores = waitSemaphores.data();
    presentInfo.pResults = results.data();

    // Present our SwapChain images.
    VkResult result = vkQueuePresentKHR(queue->vkQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        for (uint32_t i = 0; i < numSwapChains; ++i)
        {
            VulkanSwapChain* swapChain = static_cast<VulkanSwapChain*>(ppSwapChains[i]);
            RASSERT(swapChain != nullptr);

            recreate_swap_chain(swapChain);
        }

        queue_present(queue, numSwapChains, ppSwapChains, numWaitSemaphores, ppWaitSemaphores);
        return RR_SUCCESS;

    }
    else if (result != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to present a swapchain.");
    }

    bool err = false;

    // Check if we successfully presented all swapchains.
    for (uint32_t i = 0; i < numSwapChains; ++i)
    {
        if (results[i] != VK_SUCCESS)
        {
            RLOG_ERROR("Failed to present a swapchain target.");
            err = true;
        }
    }

    if (err)
    {
        return RR_UNKNOWN_FAILURE;
    }

    _current_frame_index = (_current_frame_index + 1) % gMaxFramesAtOnce;

    // We sucessfully presented our swapchain image.
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::swap_chain_acquire_next_image(ISwapChain* pSwapChain, ISemaphore* pSignalSemaphores, IFence* pSignalFence)
{
    RASSERT(pSwapChain != nullptr);

    VulkanSwapChain* swapChain = static_cast<VulkanSwapChain*>(pSwapChain);
    VulkanSemaphore* semaphore = static_cast<VulkanSemaphore*>(pSignalSemaphores);
    VulkanFence* fence = static_cast<VulkanFence*>(pSignalFence);

    RASSERT(swapChain != nullptr);

    if (fence != nullptr)
    {
        fence->submitted = true;
    }

    if (vkAcquireNextImageKHR(_device->device,
        swapChain->swapChain,
        UINT64_MAX,
        semaphore != nullptr ? semaphore->semaphore : VK_NULL_HANDLE,
        fence != nullptr ? fence->fence : VK_NULL_HANDLE,
        &swapChain->imageIndex) != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to acquire next swapchain image.");

        if (fence != nullptr)
        {
            fence->submitted = false;
        }

        return RR_OBJECT_DOES_NOT_EXIST;
    }

    if (fence != nullptr)
    {
        if (vkWaitForFences(_device->device, 1, &fence->fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
        {
            RLOG_WARNING("Failed to wait for a fence while aquiring a swapchain image.");
        }

        vkResetFences(_device->device, 1, &fence->fence);
        fence->submitted = false;
    }

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::wait_for_fence(IFence* pFence)
{
    RASSERT(pFence != nullptr);
    VulkanFence* fence = static_cast<VulkanFence*>(pFence);

    if (!fence->submitted)
    {
        return RR_SUCCESS;
    }

    VkResult res;
    res = vkWaitForFences(_device->device, 1, &fence->fence, VK_TRUE, UINT64_MAX);
    if (res != VK_SUCCESS)
    {
        RLOG_WARNING("Failed to wait for a fence.");
        return RR_FAIL;
    }

    vkResetFences(_device->device, 1, &fence->fence);

    fence->submitted = false;
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::create_frame_buffer(VulkanFrameBuffer** ppFrameBuffer, VulkanFrameBufferCreateDesc* pDesc)
{
    RASSERT(pDesc != nullptr);
    RASSERT(pDesc->numAttachments != 0);

    VulkanFrameBuffer* fb = new VulkanFrameBuffer();

    Vector<VkImageView> attachments(pDesc->numAttachments);

    for (uint32_t i = 0; i < pDesc->numAttachments; ++i)
    {
        VulkanRenderTarget* target = static_cast<VulkanRenderTarget*>(pDesc->ppAttachments[i]);
        RASSERT(target != nullptr);

        attachments[i] = target->image->imageView;
    }

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.pNext = VK_NULL_HANDLE;
    createInfo.flags = 0;
    createInfo.renderPass = pDesc->renderPass->pass;
    createInfo.attachmentCount = pDesc->numAttachments;
    createInfo.pAttachments = attachments.data();
    createInfo.width = pDesc->width;
    createInfo.height = pDesc->height;
    createInfo.layers = pDesc->layers;

    fb->desc = *pDesc;

    // Create the framebuffer object
    if (vkCreateFramebuffer(_device->device, &createInfo, nullptr, &fb->frameBuffer) != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to create a framebuffer object.");
        delete fb;
        return RR_INITIALIZATION_FAILED;
    }

    *ppFrameBuffer = fb;

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_begin(ICommandList* pCommandList)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    // Reset this command buffer.
    vkResetCommandBuffer(list->cmdBuffer, 0);
    list->currentRenderPass = nullptr;
    list->boundPipeline = nullptr;

    if (list->descriptorSetCache != nullptr)
    {
        list->descriptorSetCache->hashToSet.clear();
        vkResetDescriptorPool(_device->device, list->descriptorSetCache->descriptorPool->descriptorPool, 0);
    }

    if (list->dynamicUniformAllocator != nullptr)
    {
        list->dynamicUniformAllocator->reset(false);
    }

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = VK_NULL_HANDLE;

    // Begin recording commands in this command buffer.
    vkBeginCommandBuffer(list->cmdBuffer, &beginInfo);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_end(ICommandList* pCommandList)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    if (list->currentRenderPass != nullptr)
    {
        cmd_end_render_pass(list);
    }

    // Flush commands before ending.
    if (list->pendingImageBarriers.size() != 0 || list->pendingBufferBarriers.size() != 0)
    {
        cmd_flush_barriers(list);
    }

    // Stop recording commands in this command list.
    VkResult result = vkEndCommandBuffer(list->cmdBuffer);
    if (result != VK_SUCCESS)
    {
        RLOG_ERROR("Failed to record commands in command list.");
    }

    list->currentRenderPass = nullptr;
    list->boundPipeline = nullptr;

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_begin_render_pass(ICommandList* pCommandList, uint32_t numColorAttachments,
    IRenderTarget** ppColorAttachments,
    IRenderTarget* pDepthStencilAttachment,
    LoadStoreOps* pLoadStoreOps)
{
    RASSERT(pCommandList != nullptr);
    RASSERT(!(numColorAttachments == 0 && pDepthStencilAttachment == nullptr));

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    if (list->currentRenderPass != nullptr)
    {
        cmd_end_render_pass(list);
    }

    // Hash the attachment formats and sample count to check if we already have an existing render pass instance.
    size_t hash = 0;

    for (uint32_t i = 0; i < numColorAttachments; ++i)
    {
        VulkanRenderTarget* target = static_cast<VulkanRenderTarget*>(ppColorAttachments[i]);
        hash = hash_bytes(&target->desc.format, sizeof(TextureFormat), hash);
        hash = hash_bytes(&target->desc.sampleCount, sizeof(uint32_t), hash);
        hash = hash_bytes(&target->desc.sampleQuality, sizeof(uint32_t), hash);
    }

    if (pDepthStencilAttachment != nullptr)
    {
        VulkanRenderTarget* target = static_cast<VulkanRenderTarget*>(pDepthStencilAttachment);
        hash = hash_bytes(&target->desc.format, sizeof(TextureFormat), hash);
        hash = hash_bytes(&target->desc.sampleCount, sizeof(uint32_t), hash);
        hash = hash_bytes(&target->desc.sampleQuality, sizeof(uint32_t), hash);
    }

    size_t hash2 = hash; // we don't need to take the load/store ops in account for frame buffer creation.
    hash = hash_bytes(pLoadStoreOps, sizeof(LoadStoreOps), hash);

    VulkanRenderPass* renderPass = nullptr;

    auto renderPassIt = get_thread_local_render_pass_cache()->hashToRenderPass.find(hash);
    if (renderPassIt != get_thread_local_render_pass_cache()->hashToRenderPass.end())
    {
        // We already have a pre-created render pass.
        renderPass = renderPassIt->second;
    }
    else
    {
        VulkanRenderPassCreateDesc rpDesc;
        rpDesc.numColorAttachments = numColorAttachments;
        rpDesc.pLoadStoreOps = pLoadStoreOps;

        if (numColorAttachments != 0)
        {
            VulkanRenderTarget* colorTarget = static_cast<VulkanRenderTarget*>(ppColorAttachments[0]);
            rpDesc.sampleCount = colorTarget->image->desc.sampleCount;
            rpDesc.sampleQuality = colorTarget->image->desc.sampleQuality;

            for (uint32_t i = 0; i < numColorAttachments; ++i)
            {
                VulkanRenderTarget* colorAttachment = static_cast<VulkanRenderTarget*>(ppColorAttachments[i]);
                rpDesc.colorAttachmentFormats[i] = colorAttachment->desc.format;
            }
        }

        if (pDepthStencilAttachment != nullptr)
        {
            VulkanRenderTarget* depthStencil = static_cast<VulkanRenderTarget*>(pDepthStencilAttachment);
            rpDesc.depthStencilAttachmentFormat = depthStencil->desc.format;

            if (numColorAttachments == 0)
            {
                rpDesc.sampleCount = depthStencil->image->desc.sampleCount;
                rpDesc.sampleQuality = depthStencil->image->desc.sampleQuality;
            }
        }
        else
        {
            rpDesc.depthStencilAttachmentFormat = TextureFormat::TEXTURE_FORMAT_UNKNOWN;
        }

        // create a new render pass.
        if (create_render_pass(&renderPass, &rpDesc) != RR_SUCCESS)
        {
            RLOG_ERROR("Failed to create a render pass for the specified formats.");
            return RR_FAIL;
        }

        get_thread_local_render_pass_cache()->hashToRenderPass[hash] = renderPass;
    }

    VulkanFrameBuffer* frameBuffer = nullptr;

    for (uint32_t i = 0; i < numColorAttachments; ++i)
    {
        VulkanRenderTarget* target = static_cast<VulkanRenderTarget*>(ppColorAttachments[i]);
        hash2 = hash_bytes(&target->image->textureId, sizeof(size_t), hash2);
    }

    if (pDepthStencilAttachment != nullptr)
    {
        VulkanRenderTarget* target = static_cast<VulkanRenderTarget*>(pDepthStencilAttachment);
        hash2 = hash_bytes(&target->image->textureId, sizeof(size_t), hash2);
    }

    auto frameBufferIt = get_thread_local_framebuffer_cache()->hashToBuffer.find(hash2);
    if (frameBufferIt != get_thread_local_framebuffer_cache()->hashToBuffer.end())
    {
        // We already have a pre-created render pass.
        frameBuffer = frameBufferIt->second;
    }
    else
    {
        VulkanFrameBufferCreateDesc fbDesc;
        fbDesc.renderPass = renderPass;

        Vector<VulkanRenderTarget*> attachments(numColorAttachments + (pDepthStencilAttachment != nullptr ? 1 : 0));

        if (numColorAttachments != 0)
        {
            VulkanRenderTarget* colorTarget = static_cast<VulkanRenderTarget*>(ppColorAttachments[0]);
            fbDesc.width = colorTarget->image->desc.width;
            fbDesc.height = colorTarget->image->desc.height;
            fbDesc.layers = colorTarget->image->desc.arrayLayers;

            for (uint32_t i = 0; i < numColorAttachments; ++i)
            {
                VulkanRenderTarget* colorAttachment = static_cast<VulkanRenderTarget*>(ppColorAttachments[i]);
                attachments[i] = colorAttachment;
            }
        }

        if (pDepthStencilAttachment != nullptr)
        {
            VulkanRenderTarget* depthAttachment = static_cast<VulkanRenderTarget*>(pDepthStencilAttachment);
            fbDesc.width = depthAttachment->image->desc.width;
            fbDesc.height = depthAttachment->image->desc.height;
            fbDesc.layers = depthAttachment->image->desc.arrayLayers;
            attachments[numColorAttachments] = depthAttachment;
        }

        fbDesc.numAttachments = (uint32_t)attachments.size();
        fbDesc.ppAttachments = attachments.data();

        // Create a new frame buffer referencing the resources.
        if (create_frame_buffer(&frameBuffer, &fbDesc) != RR_SUCCESS)
        {
            RLOG_ERROR("Failed to create a frame buffer for the specified targets.");
            return RR_FAIL;
        }

        get_thread_local_framebuffer_cache()->hashToBuffer[hash2] = frameBuffer;

    }

    RASSERT(frameBuffer != nullptr);
    RASSERT(renderPass != nullptr);

    VkRect2D renderArea;
    renderArea.offset.x = 0;
    renderArea.offset.y = 0;
    renderArea.extent.width = frameBuffer->desc.width;
    renderArea.extent.height = frameBuffer->desc.height;

    VkRenderPassBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = VK_NULL_HANDLE;
    beginInfo.renderPass = renderPass->pass;
    beginInfo.framebuffer = frameBuffer->frameBuffer;
    beginInfo.renderArea = renderArea;

    Vector<VkClearValue> clearValues(numColorAttachments + (pDepthStencilAttachment != nullptr ? 1 : 0));
    for (uint32_t i = 0; i < numColorAttachments; ++i)
    {
        memcpy(&clearValues[i].color, pLoadStoreOps->colorClearValues[i], sizeof(float) * 4);
    }

    if (pDepthStencilAttachment != nullptr)
    {
        clearValues[numColorAttachments].depthStencil.depth = pLoadStoreOps->depthClearValue;
        clearValues[numColorAttachments].depthStencil.stencil = pLoadStoreOps->stencilClearValue;
    }

    beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    beginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(list->cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    list->currentRenderPass = renderPass;
    //
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_end_render_pass(ICommandList* pCommandList)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    if (list->currentRenderPass != nullptr)
    {
        vkCmdEndRenderPass(list->cmdBuffer);
        list->currentRenderPass = nullptr;
        return RR_SUCCESS;
    }

    return RR_INITIALIZATION_FAILED;
}

RenderResult VulkanRenderer::cmd_bind_vertex_buffer(ICommandList* pCommandList, uint32_t numBuffers, IBuffer** ppBuffers,
    uint32_t startSlot) {
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    Vector<VkBuffer> buffers(numBuffers);
    Vector<VkDeviceSize > offsets(numBuffers);

    // Fill the buffers and offsets vectors with vulkan data.
    for (uint32_t i = 0; i < numBuffers; ++i)
    {
        VulkanBuffer* buffer = static_cast<VulkanBuffer*>(ppBuffers[i]);

        buffers[i] = buffer->buffer;
        offsets[i] = 0;
    }

    vkCmdBindVertexBuffers(list->cmdBuffer, startSlot, numBuffers, buffers.data(), offsets.data());
    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_bind_index_buffer(ICommandList* pCommandList, IBuffer* pBuffer, IndexType type)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);
    VulkanBuffer* buffer = static_cast<VulkanBuffer*>(pBuffer);

    vkCmdBindIndexBuffer(list->cmdBuffer, buffer->buffer, 0, gIndexTypeTranslator[type]);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_set_viewport(ICommandList* pCommandList, Rect rect, float minDepth, float maxDepth)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    VkViewport viewport;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    viewport.x = rect.position.x();
    viewport.y = rect.position.y();
    viewport.width = rect.size.x();
    viewport.height = rect.size.y();

    vkCmdSetViewport(list->cmdBuffer, 0, 1, &viewport);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_set_dynamic_depth_bias(ICommandList* pCommandList, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    vkCmdSetDepthBias(list->cmdBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_set_scissor_rects(ICommandList* pCommandList, Rect rect)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    VkRect2D vkRect = { {(int32_t)rect.position.x(), (int32_t)rect.position.y()}, {(uint32_t)rect.size.x(), (uint32_t)rect.size.y()} };
    vkCmdSetScissor(list->cmdBuffer, 0, 1, &vkRect);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_bind_pipeline(ICommandList* pCommandList, IPipeline* pPipeline)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);
    VulkanPipeline* pipeline = static_cast<VulkanPipeline*>(pPipeline);

    // This pipeline is already bound so we don't have to re-bind.
    if (list->boundPipeline == pipeline)
    {
        return RR_SUCCESS;
    }

    list->boundPipeline = pipeline;
    vkCmdBindPipeline(list->cmdBuffer, gPipelineTypeTranslator[pipeline->type], pipeline->pipeline);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_resource_barriers(ICommandList* pCommandList, uint32_t numBufferBarriers,
    BufferBarrier* pBufferBarriers, uint32_t numTextureBarries,
    ImageBarrier* pTextureBarriers, bool flush)
{
    RASSERT(pCommandList != nullptr);
    RASSERT(numBufferBarriers < MAX_RESOURCE_BARRIERS);
    RASSERT(numTextureBarries < MAX_RESOURCE_BARRIERS);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    if (numBufferBarriers == 0 && numTextureBarries == 0)
    {
        RLOG_WARNING("For some reason we call resource barriers without any barriers, so we have no need to call the command.");
        return RR_SUCCESS;
    }

    // If we want to record more barriers than we can hold at a single time, flush the current pending barriers.
    if (numBufferBarriers + list->pendingBufferBarriers.size() > MAX_RESOURCE_BARRIERS
        || numTextureBarries + list->pendingImageBarriers.size() > MAX_RESOURCE_BARRIERS)
    {
        cmd_flush_barriers(pCommandList);
    }

    for (uint32_t i = 0; i < numTextureBarries; ++i)
    {
        ImageBarrier* barrier = &pTextureBarriers[i];
        VulkanImage* image = static_cast<VulkanImage*>(barrier->image);

        auto pendingBarrier = list->pendingImageBarriers.find(image);
        if (pendingBarrier != list->pendingImageBarriers.end())
        {
            // We already have a barrier which applies to this image.
            std::get<1>(pendingBarrier->second) = barrier->requiredState;
        }
        // If we don't have an existing transition, create a new one instead.
        else
        {
            list->pendingImageBarriers[image] = std::make_tuple(image->state, barrier->requiredState);
        }
    }

    for (uint32_t i = 0; i < numBufferBarriers; ++i)
    {
        BufferBarrier* barrier = &pBufferBarriers[i];
        VulkanBuffer* buffer = static_cast<VulkanBuffer*>(barrier->buffer);

        bool hasExistingTransition = false;

        auto pendingBarrier = list->pendingBufferBarriers.find(buffer);
        if (pendingBarrier != list->pendingBufferBarriers.end())
        {
            // We already have a barrier which applies to this image.
            std::get<1>(pendingBarrier->second) = barrier->requiredState;

        }
        // If we don't have an existing transition, create a new one instead.
        else
        {
            list->pendingBufferBarriers[buffer] = std::make_tuple(buffer->state, barrier->requiredState);
        }
    }

    // Call the flush command if we want to immediately record these resource barriers into the command buffer.
    if (flush)
    {
        cmd_flush_barriers(pCommandList);
    }

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_flush_barriers(ICommandList* pCommandList)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    Vector<VkBufferMemoryBarrier> bufferBarriers(list->pendingBufferBarriers.size());
    Vector<VkImageMemoryBarrier> imageBarriers(list->pendingImageBarriers.size());

    uint32_t index = 0;

    for (auto it = list->pendingBufferBarriers.begin(); it != list->pendingBufferBarriers.end(); ++it)
    {
        VkBufferMemoryBarrier* vkBarrier = &bufferBarriers[index];
        vkBarrier->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        vkBarrier->pNext = VK_NULL_HANDLE;
        vkBarrier->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBarrier->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBarrier->srcAccessMask = translateResourceStateToAccessMask(it->first->state);
        vkBarrier->dstAccessMask = translateResourceStateToAccessMask(std::get<1>(it->second));
        vkBarrier->offset = 0;
        vkBarrier->size = VK_WHOLE_SIZE;
        vkBarrier->buffer = it->first->buffer;

        it->first->state = std::get<1>(it->second);
        ++index;
    }

    index = 0;
    for (auto it = list->pendingImageBarriers.begin(); it != list->pendingImageBarriers.end(); ++it)
    {
        VkImageMemoryBarrier* vkBarrier = &imageBarriers[index];
        vkBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        vkBarrier->image = it->first->image;
        vkBarrier->pNext = VK_NULL_HANDLE;
        vkBarrier->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBarrier->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBarrier->srcAccessMask = translateResourceStateToAccessMask(it->first->state);
        vkBarrier->dstAccessMask = translateResourceStateToAccessMask(std::get<1>(it->second));
        vkBarrier->oldLayout = translateResourceStateToImageLayout(it->first->state);
        vkBarrier->newLayout = translateResourceStateToImageLayout(std::get<1>(it->second));
        vkBarrier->subresourceRange.aspectMask = it->first->vkImageViewDesc.subresourceRange.aspectMask;
        vkBarrier->subresourceRange.baseMipLevel = 0;
        vkBarrier->subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        vkBarrier->subresourceRange.baseArrayLayer = 0;
        vkBarrier->subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        it->first->state = std::get<1>(it->second);

        ++index;
    }

    list->pendingBufferBarriers.clear();
    list->pendingImageBarriers.clear();

    // Flush pipeline barrier.
    vkCmdPipelineBarrier(list->cmdBuffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0,
        0,
        nullptr,
        (uint32_t)bufferBarriers.size(),
        bufferBarriers.data(),
        (uint32_t)imageBarriers.size(),
        imageBarriers.data()
    );

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_draw(ICommandList* pCommandList, uint32_t vertexCount, uint32_t vertexOffset)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    RASSERT(list->boundPipeline != nullptr);

    if (list->boundPipeline != nullptr && list->boundPipeline->type != PipelineType::PIPELINE_TYPE_GRAPHICS)
    {
        RLOG_WARNING("Tried to call a draw command on a command list where the currently bound pipeline is not a graphics pipeline.");
        return RR_FAIL;
    }

    vkCmdDraw(list->cmdBuffer, vertexCount, 1, vertexOffset, 0);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_draw_indexed(ICommandList* pCommandList, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    RASSERT(list->boundPipeline != nullptr);

    if (list->boundPipeline != nullptr && list->boundPipeline->type != PipelineType::PIPELINE_TYPE_GRAPHICS)
    {
        RLOG_WARNING("Tried to call a draw command on a command list where the currently bound pipeline is not a graphics pipeline.");
        return RR_FAIL;
    }

    vkCmdDrawIndexed(list->cmdBuffer, indexCount, 1, indexOffset, vertexOffset, 0);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_draw_instanced(ICommandList* pCommandList, uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    RASSERT(list->boundPipeline != nullptr);

    if (list->boundPipeline != nullptr && list->boundPipeline->type != PipelineType::PIPELINE_TYPE_GRAPHICS)
    {
        RLOG_WARNING("Tried to call a draw command on a command list where the currently bound pipeline is not a graphics pipeline.");
        return RR_FAIL;
    }

    vkCmdDraw(list->cmdBuffer, vertexCount, instanceCount, 0, 0);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_draw_indexed_instanced(ICommandList* pCommandList, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset, uint32_t instanceCount)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    RASSERT(list->boundPipeline != nullptr);

    if (list->boundPipeline != nullptr && list->boundPipeline->type != PipelineType::PIPELINE_TYPE_GRAPHICS)
    {
        RLOG_WARNING("Tried to call a draw command on a command list where the currently bound pipeline is not a graphics pipeline.");
        return RR_FAIL;
    }

    vkCmdDrawIndexed(list->cmdBuffer, indexCount, instanceCount, indexOffset, vertexOffset, 0);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_dispatch(ICommandList* pCommandList, uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ)
{
    RASSERT(pCommandList != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    RASSERT(list->boundPipeline != nullptr);

    if (list->boundPipeline != nullptr && list->boundPipeline->type != PipelineType::PIPELINE_TYPE_COMPUTE)
    {
        RLOG_WARNING("Tried to call a dispatch command on a command list where the currently bound pipeline is not a compute pipeline.");
        return RR_FAIL;
    }

    // Call dispatch command.
    vkCmdDispatch(list->cmdBuffer, numGroupsX, numGroupsY, numGroupsZ);

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_allocate_dynamic_uniform_buffer(ICommandList* pCommandList, DynamicUniformAllocation* pAllocation, uint32_t size, void* data)
{
    RASSERT(pCommandList != nullptr);
    RASSERT(pAllocation != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    // Create dynamic uniform allocator.
    if (list->dynamicUniformAllocator == nullptr)
    {
        list->dynamicUniformAllocator = new VulkanDynamicUniformAllocator(this);
    }

    VkDeviceSize alignment = _device->physicalDevice->properties.limits.minUniformBufferOffsetAlignment;

    // Allocate a block of memory from the dynamic uniform buffer.
    VulkanLinearAllocation allocation = list->dynamicUniformAllocator->allocate(this, size, alignment);

    if (!(size == 0 || data == nullptr))
    {
        // Copy the data to the buffer.
        cmd_update_buffer(pCommandList, allocation.buffer, data, allocation.offset, size);
    }

    pAllocation->buffer = allocation.buffer;
    pAllocation->offset = allocation.offset;
    pAllocation->range = allocation.size;

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_bind_descriptors(ICommandList* pCommandList, uint32_t nudescriptors, DescriptorBinding* pDescriptors)
{
    RASSERT(pCommandList != nullptr);
    if (nudescriptors == 0)
    {
        return RenderResult::RR_SUCCESS;
    }

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    // We need to have a pipeline and render pass bound to the pipeline.
    if (list->boundPipeline == nullptr || list->currentRenderPass == nullptr)
    {
        return RR_FAIL;
    }

    VulkanPipeline* pipeline = list->boundPipeline;
    VulkanRenderPass* renderpass = list->currentRenderPass;

    size_t rsHash = 0;

    rsHash = hash_bytes(&pipeline->pipeline, sizeof(VkPipeline), rsHash);

    for (uint32_t i = 0; i < pipeline->rootSignature->descriptorSetLayouts.size(); ++i)
    {
        rsHash = hash_bytes(&pipeline->rootSignature->descriptorSetLayouts[i], sizeof(VkDescriptorSetLayout), rsHash);
    }

    Vector<VkDescriptorSet>* descriptorSetTemplates;
    Vector<VkDescriptorSet> dynamicDescriptorSets;

    // Try to find the descriptor set for this pipeline/descriptor set layout combination.
    auto setCacheIt = get_thread_local_descriptor_set_cache()->setMap[_current_frame_index].find(rsHash);

    if (setCacheIt == get_thread_local_descriptor_set_cache()->setMap[_current_frame_index].end())
    {
        get_thread_local_descriptor_set_cache()->setMap[_current_frame_index][rsHash] = Vector<VkDescriptorSet>(pipeline->rootSignature->descriptorSetLayouts.size());

        VkDescriptorSetAllocateInfo dsAllocInfo;
        dsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        dsAllocInfo.pNext = VK_NULL_HANDLE;
        dsAllocInfo.descriptorPool = get_thread_local_descriptor_pool()->descriptorPool;
        dsAllocInfo.descriptorSetCount = static_cast<uint32_t>(pipeline->rootSignature->descriptorSetLayouts.size());
        dsAllocInfo.pSetLayouts = pipeline->rootSignature->descriptorSetLayouts.data();

        vkAllocateDescriptorSets(_device->device, &dsAllocInfo, get_thread_local_descriptor_set_cache()->setMap[_current_frame_index][rsHash].data());
        descriptorSetTemplates = &get_thread_local_descriptor_set_cache()->setMap[_current_frame_index][rsHash];
    }
    else
    {
        descriptorSetTemplates = &setCacheIt->second;
    }

    // Create list of descriptor sets we have to update.
    Vector<VkWriteDescriptorSet> writeSets;
    Vector<VkCopyDescriptorSet> copySets;
    Vector<uint32_t> copySetIds;
    Vector<std::unique_ptr<Vector<VkDescriptorBufferInfo>>> descriptorBufferInfos;
    Vector<std::unique_ptr<Vector<VkDescriptorImageInfo>>> descriptorImageInfos;
    std::map<uint32_t, Vector<uint32_t>> mDynamicOffsets;

    for (uint32_t i = 0; i < nudescriptors; ++i)
    {
        DescriptorBinding& binding = pDescriptors[i];

        bool requireAddCopySetIds = true;

        // Check if we need to add this set to pending copy sets.
        for (uint32_t j = 0; j < copySetIds.size(); ++j)
        {
            if (copySetIds[j] == binding.set)
            {
                requireAddCopySetIds = false;
                break;
            }
        }

        if (requireAddCopySetIds)
        {
            copySetIds.push_back(binding.set);
        }

        VkWriteDescriptorSet writeSet;
        writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeSet.pNext = VK_NULL_HANDLE;
        writeSet.descriptorCount = binding.numDescriptors;
        writeSet.descriptorType = gDescriptorTypeTranslator[binding.type];
        writeSet.dstBinding = binding.binding;
        writeSet.dstSet = (*descriptorSetTemplates)[binding.set];
        writeSet.dstArrayElement = 0;

        switch (binding.type)
        {
        case DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
        case DescriptorType::DESCRIPTOR_TYPE_STORAGE_BUFFER:
        case DescriptorType::DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
        {
            uint32_t firstVector = static_cast<uint32_t>(descriptorBufferInfos.size());
            descriptorBufferInfos.push_back(std::unique_ptr<Vector<VkDescriptorBufferInfo>>(new Vector<VkDescriptorBufferInfo>()));

            for (uint32_t j = 0; j < binding.numDescriptors; ++j)
            {
                descriptorBufferInfos[firstVector]->push_back({});
                VkDescriptorBufferInfo& bufferInfo = descriptorBufferInfos[firstVector]->back();
                bufferInfo.buffer = static_cast<VulkanBuffer*>(binding.ppBuffers[j])->buffer;
                bufferInfo.offset = binding.type == DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ? 0 : binding.offset;
                bufferInfo.range = (binding.range == 0 ? VK_WHOLE_SIZE : binding.range);
            }

            if (binding.type == DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
            {
                mDynamicOffsets[binding.set].push_back(binding.offset);
            }

            writeSet.pBufferInfo = &descriptorBufferInfos[firstVector].get()->at(0);
        }
        break;
        case DescriptorType::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
        case DescriptorType::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
        {
            RLOG_ERROR("Texel buffers are unimplemented");
        }
        break;
        case DescriptorType::DESCRIPTOR_TYPE_SAMPLER:
        {
            RLOG_ERROR("Tried to bind a sampler, but a sampler should be compiled with the descriptor set layout.");
            return RR_FAIL;
        }
        break;
        case DescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case DescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE:
        {
            uint32_t firstVector = static_cast<uint32_t>(descriptorImageInfos.size());
            descriptorImageInfos.push_back(std::unique_ptr<Vector<VkDescriptorImageInfo>>(new Vector<VkDescriptorImageInfo>()));

            for (uint32_t j = 0; j < binding.numDescriptors; ++j)
            {
                descriptorImageInfos[firstVector]->push_back({});
                VkDescriptorImageInfo& imageInfo = descriptorImageInfos[firstVector]->back();
                imageInfo.imageView = static_cast<VulkanImage*>(binding.ppImages[j])->imageView;
                imageInfo.imageLayout = translateResourceStateToImageLayout(static_cast<VulkanImage*>(binding.ppImages[j])->state);
                imageInfo.sampler = VK_NULL_HANDLE;
            }

            writeSet.pImageInfo = &descriptorImageInfos[firstVector].get()->at(0);
        }
        break;

        }

        writeSets.push_back(writeSet);
    }

    if (list->descriptorSetCache == nullptr)
    {
        list->descriptorSetCache = new VulkanDynamicDescriptorSetCache();

        VulkanDescriptorPoolCreateDesc desc;
        desc.flags = 0;
        desc.maxDescriptorSets = gDynamicDescriptorSets;
        desc.numPoolSizes = VK_DESCRIPTOR_TYPE_RANGE_SIZE;
        desc.pPoolSizes = gDynamicDescriptorPoolSizes;

        create_descriptor_pool(&list->descriptorSetCache->descriptorPool, &desc);
    }

    dynamicDescriptorSets.resize(copySetIds.size());

    // Copy global descriptors into dynamic descriptors
    for (uint32_t i = 0; i < copySetIds.size(); ++i)
    {
        VkDescriptorSetAllocateInfo dsAllocInfo;
        dsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        dsAllocInfo.pNext = VK_NULL_HANDLE;
        dsAllocInfo.descriptorPool = list->descriptorSetCache->descriptorPool->descriptorPool;
        dsAllocInfo.descriptorSetCount = 1;
        dsAllocInfo.pSetLayouts = &pipeline->rootSignature->descriptorSetLayouts[copySetIds[i]];

        vkAllocateDescriptorSets(_device->device, &dsAllocInfo, &dynamicDescriptorSets[i]);
    }

    for (uint32_t i = 0; i < writeSets.size(); ++i)
    {
        uint32_t dstSet = -1;
        for (uint32_t j = 0; j < copySetIds.size(); ++j)
        {
            if ((*descriptorSetTemplates)[copySetIds[j]] == writeSets[i].dstSet)
                dstSet = j;
        }

        RASSERT(dstSet != -1);

        VkCopyDescriptorSet copySet;
        copySet.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
        copySet.pNext = VK_NULL_HANDLE;
        copySet.descriptorCount = writeSets[i].descriptorCount;
        copySet.dstArrayElement = writeSets[i].dstArrayElement;
        copySet.dstBinding = writeSets[i].dstBinding;
        copySet.dstSet = dynamicDescriptorSets[dstSet];
        copySet.srcArrayElement = writeSets[i].dstArrayElement;
        copySet.srcBinding = writeSets[i].dstBinding;
        copySet.srcSet = writeSets[i].dstSet;

        copySets.push_back(copySet);
    }

    vkUpdateDescriptorSets(_device->device, static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, nullptr);
    vkUpdateDescriptorSets(_device->device, 0, nullptr, static_cast<uint32_t>(copySets.size()), copySets.data());

    for (uint32_t i = 0; i < copySetIds.size(); ++i)
    {
        // Bind descriptor sets.
        vkCmdBindDescriptorSets(list->cmdBuffer, gPipelineTypeTranslator[pipeline->type], pipeline->layout, copySetIds[i], 1, 
            &dynamicDescriptorSets[i], mDynamicOffsets[copySetIds[i]].size(), 
            mDynamicOffsets[copySetIds[i]].empty() ? nullptr : mDynamicOffsets[copySetIds[i]].data());
    }

    return RR_SUCCESS;
}

RenderResult VulkanRenderer::cmd_push_constant(ICommandList* pCommandList, ShaderStageFlags stages, uint32_t size, uint32_t offset, void* values)
{
    RASSERT(pCommandList != nullptr);
    RASSERT(size % 4 == 0);
    RASSERT(stages != 0);
    RASSERT(offset % 4 == 0);
    RASSERT(values != nullptr);

    // Cast the command list interface to the vulkan type.
    VulkanCommandList* list = static_cast<VulkanCommandList*>(pCommandList);

    // Push constants.
    vkCmdPushConstants(list->cmdBuffer, list->boundPipeline->layout, shaderStageFlagsToVkShaderStageFlags(stages), offset, size, values);

    return RR_SUCCESS;
}

VulkanFramebufferCache* VulkanRenderer::get_thread_local_framebuffer_cache()
{
    uint64_t threadId = Thread::get_current_thread_id();

    auto poolIt = _frame_buffer_maps.find(threadId);

    if (poolIt == _frame_buffer_maps.end())
    {
        VulkanFramebufferCache* cache = new VulkanFramebufferCache();
        _frame_buffer_maps[threadId] = cache;

        return cache;
    }

    return poolIt->second;
}


VulkanDescriptorPool* VulkanRenderer::get_thread_local_descriptor_pool()
{
    auto poolIt = _descriptor_pools.find(Thread::get_current_thread_id());

    // If we don't have a descriptor pool for this thread, create one.
    if (poolIt == _descriptor_pools.end())
    {
        VulkanDescriptorPoolCreateDesc poolDesc;
        poolDesc.flags = 0; //VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Allow us to reset individual descriptors.
        poolDesc.maxDescriptorSets = gDescriptorSets;
        poolDesc.numPoolSizes = VK_DESCRIPTOR_TYPE_RANGE_SIZE;
        poolDesc.pPoolSizes = gDescriptorPoolSizes;

        VulkanDescriptorPool* pool = nullptr;

        RenderResult result = create_descriptor_pool(&pool, &poolDesc);
        RASSERT(result == RR_SUCCESS);

        _descriptor_pools[Thread::get_current_thread_id()] = pool;
        return pool;
    }
    else
    {
        return poolIt->second;
    }

}

VulkanRenderPassCache* VulkanRenderer::get_thread_local_render_pass_cache()
{
    uint64_t threadId = Thread::get_current_thread_id();

    auto poolIt = _render_pass_maps.find(threadId);

    if (poolIt == _render_pass_maps.end())
    {
        VulkanRenderPassCache* cache = new VulkanRenderPassCache();
        _render_pass_maps[threadId] = cache;

        return cache;
    }

    return poolIt->second;
}

VulkanDescriptorSetCache* VulkanRenderer::get_thread_local_descriptor_set_cache()
{
    uint64_t threadId = Thread::get_current_thread_id();

    auto poolIt = _descriptor_set_caches.find(threadId);

    if (poolIt == _descriptor_set_caches.end())
    {
        VulkanDescriptorSetCache* cache = new VulkanDescriptorSetCache();
        cache->descriptorPool = get_thread_local_descriptor_pool();

        _descriptor_set_caches[threadId] = cache;

        return cache;
    }

    return poolIt->second;
}

