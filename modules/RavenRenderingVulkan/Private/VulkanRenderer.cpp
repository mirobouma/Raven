#include "RenderingVulkan/VulkanRenderer.h"
#include "Core/Debug/LogManager.h"
#include "HAL/PlatformFileSystem.h"
#include "Core/Utility/Hash.h"
#include <iostream>
#include <set>

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#undef max
#undef min
#include <algorithm>


#ifdef PLATFORM_LINUX
#include "SDL_syswm.h"
#include <X11/Xlib-xcb.h> // for XGetXCBConnection()
#endif

namespace Raven 
{

	size_t gMaxTextureId = 0;

	constexpr VkImageType gImageTypeTranslator[ETextureType::MAX_TEXTURE_TYPE] =
	{
		VK_IMAGE_TYPE_1D,
		VK_IMAGE_TYPE_2D,
		VK_IMAGE_TYPE_3D,
		VK_IMAGE_TYPE_2D,
	};

	constexpr VkImageTiling gImageTilingTranslator[EImageTiling::MAX_IMAGE_TILING] =
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

	VkImageUsageFlags textureUsageFlagsToVkImageFlags(FTextureUsageFlags flags)
	{
		VkImageUsageFlags rflags = 0;

		if ((flags & FTextureUsageFlags::TEXTURE_USAGE_COLOR_ATTACHMENT) != 0)
		{
			rflags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}

		if ((flags & FTextureUsageFlags::TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT) != 0)
		{
			rflags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		if ((flags & FTextureUsageFlags::TEXTURE_USAGE_INPUT_ATTACHMENT) != 0)
		{
			rflags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		}

		if ((flags & FTextureUsageFlags::TEXTURE_USAGE_SAMPLED) != 0)
		{
			rflags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}

		if ((flags & FTextureUsageFlags::TEXTURE_USAGE_STORAGE) != 0)
		{
			rflags |= VK_IMAGE_USAGE_STORAGE_BIT;
		}

		if ((flags & FTextureUsageFlags::TEXTURE_USAGE_TRANSFER_DST) != 0)
		{
			rflags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		if ((flags & FTextureUsageFlags::TEXTURE_USAGE_TRANSFER_SRC) != 0)
		{
			rflags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		if ((flags & FTextureUsageFlags::TEXTURE_USAGE_TRANSIENT_ATTACHMENT) != 0)
		{
			rflags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		}

		return rflags;
	}

	VkMemoryPropertyFlags translateMemoryPropertyFlagsToVk(FMemoryPropertyFlags flags)
	{
		VkMemoryPropertyFlags rflags = 0;

		if ((flags & FMemoryPropertyFlags::MEMORY_PROPERTY_DEVICE_LOCAL) != 0)
		{
			rflags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}

		if ((flags & FMemoryPropertyFlags::MEMORY_PROPERTY_HOST_VISIBLE) != 0)
		{
			rflags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		}

		if ((flags & FMemoryPropertyFlags::MEMORY_PROPERTY_HOST_COHERENT) != 0)
		{
			rflags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}

		if ((flags & FMemoryPropertyFlags::MEMORY_PROPERTY_HOST_CACHED) != 0)
		{
			rflags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		}

		return rflags;
	}

	constexpr VkFormat gTextureFormatTranslator[ETextureFormat::MAX_TEXTURE_FORMAT] =
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
	
	ETextureFormat VkFormatToGenericFormat(VkFormat format)
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

	constexpr VkSamplerAddressMode gSamplerAddressModeTranslator[ESamplerAddressMode::MAX_SAMPLER_ADDRESS_MODE] =
	{
		VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
	};

	constexpr VkBorderColor gBorderColorTranslator[EBorderColor::MAX_BORDER_COLOR] =
	{
		VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
		VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
		VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
		VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		VK_BORDER_COLOR_INT_OPAQUE_WHITE
	};

	constexpr VkCompareOp gSamplerComparisonFuncTranslator[ESamplerComparisonFunc::MAX_SAMPLER_COMPARISON] =
	{
		VK_COMPARE_OP_NEVER,
		VK_COMPARE_OP_GREATER,
		VK_COMPARE_OP_GREATER_OR_EQUAL,
		VK_COMPARE_OP_LESS,
		VK_COMPARE_OP_LESS_OR_EQUAL,
		VK_COMPARE_OP_EQUAL
	};

	constexpr VkFilter gSamplerFilterTranslator[ESamplerFilter::MAX_SAMPLER_FILTER] =
	{
		VK_FILTER_LINEAR,
		VK_FILTER_NEAREST
	};

	constexpr VkSamplerMipmapMode gMipModeTranslator[EMipMode::MAX_MIP_MODE] =
	{
		VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VK_SAMPLER_MIPMAP_MODE_LINEAR
	};

	constexpr VkShaderStageFlagBits gShaderStageTranslator[EShaderStage::MAX_SHADER_STAGE] =
	{
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
		VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
		VK_SHADER_STAGE_GEOMETRY_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		VK_SHADER_STAGE_COMPUTE_BIT,
	};

	constexpr VkVertexInputRate gInputRateTranslator[EVertexInputRate::MAX_VERTEX_INPUT_RATE] =
	{
		VK_VERTEX_INPUT_RATE_VERTEX,
		VK_VERTEX_INPUT_RATE_INSTANCE
	};

	constexpr VkIndexType gIndexTypeTranslator[EIndexType::MAX_INDEX_TYPE] =
	{
		VK_INDEX_TYPE_UINT16,
		VK_INDEX_TYPE_UINT32
	};

	/*! Enumeration describing a descriptor type. */
	constexpr VkDescriptorType  gDescriptorTypeTranslator[EDescriptorType::MAX_DESCRIPTOR_TYPE]
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

	constexpr VkPipelineBindPoint gPipelineTypeTranslator[EPipelineType::MAX_PIPELINE_TYPE]
	{
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		VK_PIPELINE_BIND_POINT_COMPUTE
	};

	VkShaderStageFlags shaderStageFlagsToVkShaderStageFlags(FShaderStageFlags flags)
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

	VkAccessFlags translateResourceStateToAccessMask(FResourceState state)
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
		if ((state & RESOURCE_STATE_SHADER_RESOURCE) || (state & RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)) {
			rflags |= VK_ACCESS_SHADER_READ_BIT;
		}
		if (state & RESOURCE_STATE_PRESENT) {
			rflags |= VK_ACCESS_MEMORY_READ_BIT;
		}

		return rflags;
	}

	VkImageLayout translateResourceStateToImageLayout(FResourceState state)
	{
		if ((state & RESOURCE_STATE_COPY_SOURCE) != 0)
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		if ((state & RESOURCE_STATE_COPY_DEST) != 0)
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		if ((state & RESOURCE_STATE_RENDER_TARGET) != 0)
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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

	constexpr VkCommandBufferLevel gCommandListTypeTranslator[ECommandListType::MAX_COMMAND_LIST] =
	{
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		VK_COMMAND_BUFFER_LEVEL_SECONDARY
	};

	constexpr VkPrimitiveTopology gPrimitiveTopologyTranslator[EPrimitiveTopology::MAX_PRIMITIVE_TOPOLOGY] =
	{
		VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
	};

	constexpr VkPolygonMode gPolygonModeTranslator[EPolygonMode::MAX_POLYGON_MODE] =
	{
		VK_POLYGON_MODE_FILL,
		VK_POLYGON_MODE_LINE,
		VK_POLYGON_MODE_POINT
	};

	constexpr VmaMemoryUsage gMemoryTypeTranslator[EMemoryType::MEMORY_TYPE_MAX] =
	{
		VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY,
		VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
		VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
		VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_TO_CPU,
	};

	constexpr VkCullModeFlags gCullModeTranslator[ECullMode::MAX_CULL_MODE] =
	{
		VK_CULL_MODE_NONE,
		VK_CULL_MODE_FRONT_BIT,
		VK_CULL_MODE_BACK_BIT,
		VK_CULL_MODE_FRONT_AND_BACK
	};

	constexpr VkFrontFace gFrontFaceTranslator[EFrontFaceWinding::MAX_FRONT_FACE_WINDING] =
	{
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FRONT_FACE_CLOCKWISE
	};

	constexpr VkBlendFactor gBlendFactorTranslator[EBlendFactor::MAX_BLEND_FACTOR] =
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

	constexpr VkBlendOp gBlendOpTranslator[EBlendOp::MAX_BLEND_OP] =
	{
		VK_BLEND_OP_ADD,
		VK_BLEND_OP_SUBTRACT,
		VK_BLEND_OP_REVERSE_SUBTRACT,
		VK_BLEND_OP_MIN,
		VK_BLEND_OP_MAX
	};

	constexpr VkLogicOp gLogicOpTranslator[ELogicOp::MAX_LOGIC_OP] =
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

	constexpr VkCompareOp gCompareOpTranslator[ECompareOp::MAX_COMPARE_OP] =
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

	constexpr VkAttachmentLoadOp gLoadOpTranslator[ELoadOp::MAX_LOAD_OP] =
	{
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE
	};

	constexpr VkAttachmentStoreOp gStoreOpTranslator[EStoreOp::MAX_STORE_OP] =
	{
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		
	};

	constexpr VkStencilOp gStencilOpTranslator[EStencilOp::MAX_STENCIL_OP] =
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

	VkBufferUsageFlags bufferUsageFlagsToVkBufferFlags(FBufferUsageFlags flags)
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

	ERenderResult RVulkanRenderer::initialize(FRendererInitializationDesc* pDesc)
	{
		// Don't try to initialize if we already successfully initialized before.
		if (isInitialized())
		{
			return RR_ALREADY_INITIALIZED;
		}

		// Local variable for storing results of initialization functions.
		ERenderResult result;

		// Create a vulkan instance.
		result = createVulkanInstance(pDesc);
		if (result != RR_SUCCESS)
		{
			return result;
		}

		memcpy(&mRendererInfo, pDesc, sizeof(FRendererInitializationDesc));

		// Set up debugging callbacks
		if (pDesc->enableDebugging)
		{
			result = setupDebugCallback();
			if (result != RR_SUCCESS)
			{
				return result;
			}
		}

		// Query all available GPU's and store them in the mGPUs vector.
		result = queryAvailablePhysicalDevices();
		if (result != RR_SUCCESS)
		{
			return result;
		}

		// Pick most suitable physical device.
		FVulkanPhysicalDevice* PhysicalDevice;
		result = pickPhysicalDevice(&PhysicalDevice);
		if (result != RR_SUCCESS)
		{
			return result;
		}

		// Create logical device.
		result = createLogicalDevice(PhysicalDevice);
		if (result != RR_SUCCESS)
		{
			return result;
		}

		// Create allocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = mDevice->mPhysicalDevice->mPhysicalDevice;
		allocatorInfo.device = mDevice->mDevice;

		vmaCreateAllocator(&allocatorInfo, &mAllocator);

		FVulkanDescriptorPoolCreateDesc poolDesc;
		poolDesc.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Allow us to reset individual descriptors.
		poolDesc.maxDescriptorSets = gDescriptorSets;
		poolDesc.numPoolSizes = VK_DESCRIPTOR_TYPE_RANGE_SIZE;
		poolDesc.pPoolSizes = gDescriptorPoolSizes;

		result = createDescriptorPool(&mDescriptorPool, &poolDesc);
		if (result != RR_SUCCESS)
		{
			return result;
		}

		// The renderer has been properly initialized, therefore we can set the initialized member to true.
		mInitialized = true;

		// Create transfer queue.
		FQueueCreateDesc queueDesc;
		queueDesc.type = ECommandType::COMMAND_TYPE_COPY;

		result = createQueue((IQueue**)&mTransferQueue, &queueDesc);
		if (result != RR_SUCCESS)
		{
			mInitialized = false;
			return result;
		}

		result = createCommandPool((ICommandPool**)&mTransferCommandPool, mTransferQueue, nullptr);
		if (result != RR_SUCCESS)
		{
			mInitialized = false;
			return result;
		}

		return RR_SUCCESS;
	}

	bool RVulkanRenderer::isInitialized() const
	{
		return mInitialized;
	}

	ERenderResult RVulkanRenderer::createVulkanInstance(FRendererInitializationDesc* pDesc)
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
		std::vector<const char*> validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		// We always require the surface extension to be present, and an surface extension for our platform.
		std::vector<const char*> instanceExtensions;
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

			if (enableValidationLayers(createInfo, validationLayers) != RR_SUCCESS)
			{
				return RR_LAYER_NOT_PRESENT;
			}
		}

		// Enable extensions
		if (enableInstanceExtensions(createInfo, instanceExtensions) != RR_SUCCESS)
		{
			return RR_EXTENSION_NOT_PRESENT;
		}

		// Create the vulkan instance using our vulkan create info structure. We don't define an allocator for now.
		if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
			return RR_INITIALIZATION_FAILED;
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::enableValidationLayers(VkInstanceCreateInfo& rInfo, const std::vector<const char*>& rInstanceLayers)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		// Retrieve available layers into the availablelayers vector.
		std::vector<VkLayerProperties> availableLayers(layerCount);
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

	ERenderResult RVulkanRenderer::enableInstanceExtensions(VkInstanceCreateInfo& rInfo, const std::vector<const char*>& rInstanceExtensions)
	{
		// Query how many extensions are available to us.
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		// Container of extensions.
		std::vector<VkExtensionProperties> extensions(extensionCount);

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

	VkResult RVulkanRenderer::createDebugReportCallbackExtension(VkInstance pInstance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
	{
		auto func = GET_VK_FUNC(mInstance, vkCreateDebugReportCallbackEXT);
		if (func != nullptr) {
			return func(mInstance, pCreateInfo, pAllocator, pCallback);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
	{

		RLOG_WARNING("Vulkan Validation: %s", msg);
		return VK_FALSE;
	}

	ERenderResult RVulkanRenderer::destroyDebugReportCallbackExtention(VkInstance pInstance, VkDebugReportCallbackEXT pCallback, const VkAllocationCallbacks* pAllocator) {
		auto func = GET_VK_FUNC(mInstance, vkDestroyDebugReportCallbackEXT);
		if (func != nullptr) {
			func(mInstance, pCallback, pAllocator);
			return RR_SUCCESS;
		}

		return RR_EXTENSION_NOT_PRESENT;
	}

	ERenderResult RVulkanRenderer::querySwapchainSupport(FVulkanDevice* pDevice, FVulkanSwapChain* pSwapChain)
	{
		// Retrieve surface capabilities.
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice->mPhysicalDevice->mPhysicalDevice, pSwapChain->mSurface, &pSwapChain->mSurfaceCapabilities);

		// Retrieve surface formats for the created surface.
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice->mPhysicalDevice->mPhysicalDevice, pSwapChain->mSurface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			pSwapChain->mSurfaceFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice->mPhysicalDevice->mPhysicalDevice, pSwapChain->mSurface, &formatCount, pSwapChain->mSurfaceFormats.data());
		}
		else
		{
			RLOG_ERROR("The specified surface has no surface formats.");
			return RR_FEATURE_NOT_PRESENT;
		}

		// Retrieve present modes for this surface.
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice->mPhysicalDevice->mPhysicalDevice, pSwapChain->mSurface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			pSwapChain->mSurfacePresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice->mPhysicalDevice->mPhysicalDevice, pSwapChain->mSurface, &presentModeCount, pSwapChain->mSurfacePresentModes.data());
		}
		else
		{
			RLOG_ERROR("The specified surface has no present modes.");
			return RR_FEATURE_NOT_PRESENT;
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::chooseSwapChainSurfaceFormat(FVulkanSwapChain* pSwapChain)
	{
		RASSERT(pSwapChain->mSurfaceFormats.size() != 0);

		// If we can choose the format, set it to standard SRGB.
		if (pSwapChain->mSurfaceFormats.size() == 1 && pSwapChain->mSurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			pSwapChain->mSurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM,  VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			return RR_SUCCESS;
		}

		// Otherwise, go through the available formats and pick the first format that has the format and color space we want.
		for (const auto& availableFormat : pSwapChain->mSurfaceFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				pSwapChain->mSurfaceFormat = availableFormat;
				return RR_SUCCESS;
			}
		}

		// If all else fails, we'll use the first format.
		pSwapChain->mSurfaceFormat = pSwapChain->mSurfaceFormats[0];
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::chooseSwapChainPresentMode(FVulkanSwapChain* pSwapChain)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		// Mailbox allows for triple buffering with lower latency issues compared to traditional FIFO (vsync) double buffering.
		for (const auto& availablePresentMode : pSwapChain->mSurfacePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				pSwapChain->mSurfacePresentMode = availablePresentMode;
				return RR_SUCCESS;
			}
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				bestMode = availablePresentMode;
			}
		}

		pSwapChain->mSurfacePresentMode = bestMode;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::chooseSwapChainExtent(FVulkanSwapChain* pSwapChain)
	{
		if (pSwapChain->mSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
		{
			pSwapChain->mSwapChainExtent = pSwapChain->mSurfaceCapabilities.currentExtent;
			return RR_SUCCESS;
		}
		else
		{
			VkExtent2D actualExtent = { 1024, 896 };

			actualExtent.width = std::max(pSwapChain->mSurfaceCapabilities.minImageExtent.width,
										  std::min(pSwapChain->mSurfaceCapabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(pSwapChain->mSurfaceCapabilities.minImageExtent.height,
										   std::min(pSwapChain->mSurfaceCapabilities.maxImageExtent.height, actualExtent.height));

			pSwapChain->mSwapChainExtent = actualExtent;

			return RR_SUCCESS;
		}
	}

	ERenderResult RVulkanRenderer::createWindowSurface(Raven::IPlatformWindow* pWindow, VkSurfaceKHR* pSurface)
	{
#if defined(PLATFORM_WINDOWS)
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = (HWND)pWindow->GetNativeHandle();
		createInfo.hinstance = GetModuleHandle(nullptr);

		auto CreateWin32SurfaceKHR = GET_VK_FUNC(mInstance, vkCreateWin32SurfaceKHR);

		if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(mInstance, &createInfo, nullptr, pSurface) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create window surface!");
			return RR_INITIALIZATION_FAILED;
		}

#elif defined(PLATFORM_LINUX)
		SDL_SysWMinfo wminfo;
		SDL_VERSION(&wminfo.version);
		if (!SDL_GetWindowWMInfo((SDL_Window*)pWindow->GetNativeHandle(), &wminfo))
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

		auto CreateXcbSurfaceKHR = GET_VK_FUNC(mInstance, vkCreateXcbSurfaceKHR);
		if (!CreateXcbSurfaceKHR || CreateXcbSurfaceKHR(mInstance, &createInfo, nullptr, pSurface) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create window surface!");
			return RR_INITIALIZATION_FAILED;
		}
#endif
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createSwapChain(IPlatformWindow* pNativeWindow, ISwapChain** ppSwapChain)
	{
		RASSERT(mInitialized);
		RASSERT(mDevice != nullptr);

		FVulkanSwapChain* swapChain = new FVulkanSwapChain();

		// Create surface for window.
		if(createWindowSurface(pNativeWindow, &swapChain->mSurface) != RR_SUCCESS)
		{
			RLOG_ERROR("Failed to create surface for the specified window.");
			delete swapChain;

			return RR_INITIALIZATION_FAILED;
		}

		
		VkBool32 deviceSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(mDevice->mPhysicalDevice->mPhysicalDevice, mDevice->mQueueIndices.mGraphicsQueueIndex, swapChain->mSurface, &deviceSupport);
		if (deviceSupport != VK_TRUE)
		{
			return RR_INITIALIZATION_FAILED;
		}

		// Retrieve swapchain support details into the surface object.
		if(querySwapchainSupport(mDevice, swapChain) != RR_SUCCESS)
		{
			RLOG_ERROR("We do not support swapchain creation for this device (Missing extensions?).");
			delete swapChain;

			return RR_INITIALIZATION_FAILED;
		}

		if(chooseSwapChainSurfaceFormat(swapChain) != RR_SUCCESS)
		{
			RLOG_ERROR("We could not find a suitable SwapChain surface format.");
			delete swapChain;

			return RR_INITIALIZATION_FAILED;
		}

		if(chooseSwapChainPresentMode(swapChain) != RR_SUCCESS)
		{
			RLOG_ERROR("We could not find a suitable SwapChain present mode.");
			delete swapChain;

			return RR_INITIALIZATION_FAILED;
		}

		if(chooseSwapChainExtent(swapChain) != RR_SUCCESS)
		{
			RLOG_ERROR("We could not find a suitable SwapChain surface format.");
			delete swapChain;

			return RR_INITIALIZATION_FAILED;
		}

		// Select how many backbuffers we need in the swap chain, we preferably want the minimum amount + 1 for minimal latency without tearing.
		uint32_t imageCount = swapChain->mSurfaceCapabilities.minImageCount + 1;
		if (swapChain->mSurfaceCapabilities.maxImageCount > 0 && imageCount > swapChain->mSurfaceCapabilities.maxImageCount)
		{
			imageCount = swapChain->mSurfaceCapabilities.maxImageCount;
		}

		// Fill the swapchain creation info structure.
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = swapChain->mSurface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = swapChain->mSurfaceFormat.format;
		createInfo.imageColorSpace = swapChain->mSurfaceFormat.colorSpace;
		createInfo.imageExtent = swapChain->mSwapChainExtent;
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

		createInfo.preTransform = swapChain->mSurfaceCapabilities.currentTransform; // We don't want any transformation so we set it to the current transform.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // We don't want to use the alpha bit for blending with other windows so set it to OPAQUE.
		createInfo.presentMode = swapChain->mSurfacePresentMode; // Set the present mode to the one we selected.
		createInfo.clipped = VK_TRUE; // We don't need to read the swapchain image, so if another window is obscuring it, we don't care about it's colours.
		createInfo.oldSwapchain = VK_NULL_HANDLE; // For now we only need 1 swapchain ever and dont care.

		if (vkCreateSwapchainKHR(mDevice->mDevice, &createInfo, nullptr, &swapChain->mSwapChain) != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to create SwapChain object.");
		}

		std::vector<VkImage>	swapChainImages; /*!< Vector containing a VkImage for each backbuffer in the SwapChain. */

		// Retrieve swap chain images.
		vkGetSwapchainImagesKHR(mDevice->mDevice, swapChain->mSwapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(mDevice->mDevice, swapChain->mSwapChain, &imageCount, swapChainImages.data());

		swapChain->mNumBackbuffers = imageCount;
		swapChain->mRenderTargets = (FVulkanRenderTarget**)malloc(sizeof(FVulkanRenderTarget*) * imageCount);

		// Assign the native window to this swapchain.
		swapChain->mPlatformWindow = pNativeWindow;

		for (uint32_t i = 0; i < swapChainImages.size(); i++)
		{
			//VkImageViewCreateInfo viewInfo = {};
			//viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			//viewInfo.image = swapChain->mSwapChainImages[i];
			//viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			//viewInfo.format = swapChain->mSurfaceFormat.format;
			//viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			//viewInfo.subresourceRange.baseMipLevel = 0;
			//viewInfo.subresourceRange.levelCount = 1;
			//viewInfo.subresourceRange.baseArrayLayer = 0;
			//viewInfo.subresourceRange.layerCount = 1;
			//
			//if (vkCreateImageView(mDevice->mDevice, &viewInfo, nullptr, &swapChain->mSwapChainImageViews[i]) != VK_SUCCESS)
			//{
			//	RLOG_ERROR("Failed to create image views for the swapchain.");
			//}

			FRenderTargetCreateDesc rtDesc = {};
			rtDesc.format = VkFormatToGenericFormat(swapChain->mSurfaceFormat.format);
			rtDesc.width = swapChain->mSwapChainExtent.width;
			rtDesc.height = swapChain->mSwapChainExtent.height;
			rtDesc.sampleCount = 1;
			rtDesc.sampleQuality = 0;
			rtDesc.usage = FRenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT;
			rtDesc.nativeSwapchainImage = swapChainImages[i];
			createRenderTarget((IRenderTarget**)(&swapChain->mRenderTargets[i]), &rtDesc);
		}

		// Update the pointer argument to point to the newly created swapchain.
		*ppSwapChain = (ISwapChain*)swapChain;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroySwapChain(ISwapChain* pSwapchain)
	{
		RASSERT(mDevice != nullptr);
		RASSERT(pSwapchain != nullptr);

		FVulkanSwapChain* swapChain = static_cast<FVulkanSwapChain*>(pSwapchain);

		for (uint32_t i = 0; i < swapChain->mNumBackbuffers; ++i)
		{
			vkDestroyImageView(mDevice->mDevice, swapChain->mRenderTargets[i]->mTexture->mImageView, nullptr);
		}

		free(swapChain->mRenderTargets);
		vkDestroySwapchainKHR(mDevice->mDevice, swapChain->mSwapChain, nullptr);
		vkDestroySurfaceKHR(mInstance, swapChain->mSurface, nullptr);

		delete pSwapchain;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createQueue(IQueue** ppQueue, FQueueCreateDesc* pDesc)
	{
		RASSERT(mInitialized);
		RASSERT(mDevice != nullptr);
		RASSERT(pDesc != nullptr);

		FVulkanQueue* queue = new FVulkanQueue();

		uint32_t queueFamilyIndex = UINT32_MAX;

		// Retrieve queue family index.
		switch (pDesc->type)
		{
			case ECommandType::COMMAND_TYPE_COMPUTE:
			{
				queueFamilyIndex = mDevice->mQueueIndices.mComputeQueueIndex;
			}
			break;
			case ECommandType::COMMAND_TYPE_COPY:
			{
				queueFamilyIndex = mDevice->mQueueIndices.mTransferQueueIndex;
			}
			break;
			case ECommandType::COMMAND_TYPE_DIRECT:
			case ECommandType::COMMAND_TYPE_BUNDLE:
			{
				queueFamilyIndex = mDevice->mQueueIndices.mGraphicsQueueIndex;
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
		if (mDevice->mQueueCounters.find(queueFamilyIndex) == mDevice->mQueueCounters.end())
		{
			mDevice->mQueueCounters[queueFamilyIndex] = 1;
		}
		else
		{
			// Get the first available queue index and increment the counter.
			queueIndex = mDevice->mQueueCounters[queueFamilyIndex];

			if (queueIndex >= mDevice->mPhysicalDevice->mQueueFamilyProperties[queueFamilyIndex].queueCount)
			{
				queueIndex = mDevice->mPhysicalDevice->mQueueFamilyProperties[queueFamilyIndex].queueCount - 1;
			}

			mDevice->mQueueCounters[queueFamilyIndex]++;
		}

		// Retrieve queue from the logical device.
		vkGetDeviceQueue(mDevice->mDevice, queueFamilyIndex, queueIndex, &queue->mQueue);

		queue->mDevice = mDevice;
		queue->mQueueFamilyIndex = queueFamilyIndex;
		queue->mQueueIndex = queueIndex;

		// Set the out pointer to the newly created queue.
		*ppQueue = (IQueue*)queue;

		return ERenderResult::RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyQueue(IQueue* pQueue)
	{
		FVulkanQueue* queue = static_cast<FVulkanQueue*>(pQueue);

		RASSERT(queue != nullptr);
		RASSERT(queue->mDevice != nullptr);

		// Decrement queue index.
		queue->mDevice->mQueueCounters[queue->mQueueFamilyIndex]--;

		delete pQueue;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createCommandPool(ICommandPool** ppCmdPool, IQueue* pQueue, FCommandPoolCreateDesc* pDesc)
	{
		RASSERT(mInitialized);
		RASSERT(mDevice != nullptr);

		FVulkanQueue* queue = static_cast<FVulkanQueue*>(pQueue);
		RASSERT(queue != nullptr);

		FVulkanCommandPool* pool = new FVulkanCommandPool();

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queue->mQueueFamilyIndex;
		poolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		// Create command pool for this queue.
		if (vkCreateCommandPool(mDevice->mDevice, &poolInfo, nullptr, &pool->mPool) != VK_SUCCESS) 
		{
			RLOG_ERROR("Failed to create command pool.");
			delete pool;

			return RR_INITIALIZATION_FAILED;
		}

		*ppCmdPool = (ICommandPool*)pool;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyCommandPool(ICommandPool* pCmdPool)
	{
		FVulkanCommandPool* pool = static_cast<FVulkanCommandPool*>(pCmdPool);
		RASSERT(pool != nullptr);

		vkDestroyCommandPool(mDevice->mDevice, pool->mPool, nullptr);

		delete pool;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createCommandList(ICommandList** ppCommandList, ICommandPool* pCmdPool, ECommandListType type)
	{
		return createCommandLists(ppCommandList, 1, pCmdPool, type);
	}

	ERenderResult RVulkanRenderer::createCommandLists(ICommandList** ppCommandLists, uint32_t numCommandLists, ICommandPool* pCmdPool, ECommandListType type)
	{
		RASSERT(ppCommandLists != nullptr);
		RASSERT(pCmdPool != nullptr);

		FVulkanCommandPool* pool = static_cast<FVulkanCommandPool*>(pCmdPool);

		std::vector<VkCommandBuffer> CommandBuffers;
		CommandBuffers.resize(numCommandLists);

		// Create command list wrapper objects.
		for (uint32_t i = 0; i < numCommandLists; ++i)
		{
			FVulkanCommandList* newList = new FVulkanCommandList();
			newList->mBoundPipeline = nullptr;
			newList->mCurrentRenderPass = nullptr;
			newList->mDescriptorSetCache = nullptr;

			ppCommandLists[i] = newList;
		}

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = pool->mPool;
		allocInfo.level = gCommandListTypeTranslator[type];
		allocInfo.commandBufferCount = (uint32_t)numCommandLists;

		if (vkAllocateCommandBuffers(mDevice->mDevice, &allocInfo, CommandBuffers.data()) != VK_SUCCESS) 
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
			static_cast<FVulkanCommandList*>(ppCommandLists[i])->mCmdBuffer = CommandBuffers[i];
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyCommandList(ICommandPool* pCmdPool, ICommandList* pCommandList)
	{
		return destroyCommandLists(pCmdPool, &pCommandList, 1);
	}

	ERenderResult RVulkanRenderer::destroyCommandLists(ICommandPool* pCmdPool, ICommandList** ppCommandLists, uint32_t numLists)
	{
		RASSERT(pCmdPool != nullptr);
		RASSERT(ppCommandLists != nullptr);
		RASSERT(numLists > 0);

		FVulkanCommandPool* pool = static_cast<FVulkanCommandPool*>(pCmdPool);

		std::vector<VkCommandBuffer> buffers;
		buffers.resize(numLists);

		for (uint32_t i = 0; i < numLists; ++i)
		{
			FVulkanCommandList* list = static_cast<FVulkanCommandList*>(ppCommandLists[i]);

			if (list->mDescriptorSetCache != nullptr)
			{
				destroyDescriptorPool(list->mDescriptorSetCache->mDescriptorPool);
			}

			buffers[i] = list->mCmdBuffer;
			delete list;
		}

		vkFreeCommandBuffers(mDevice->mDevice, pool->mPool, numLists, buffers.data());

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createPipeline(IPipeline** ppPipeline, FGraphicsPipelineCreateDesc* pDesc)
	{
		RASSERT(pDesc != nullptr);
		RASSERT(mInitialized);
		RASSERT(pDesc->pShader != nullptr);
		RASSERT(pDesc->pRootSignature != nullptr);
		RASSERT(pDesc->numRenderTargets <= 8);

		FVulkanPipeline* pipeline = new FVulkanPipeline();
		pipeline->mType = PIPELINE_TYPE_GRAPHICS;

		// Create pipeline shader stages.
		VkPipelineShaderStageCreateInfo shaderStages[MAX_SHADER_STAGE];
		uint32_t numShaderStages = 0;

		FVulkanShader* shader = static_cast<FVulkanShader*>(pDesc->pShader);
		FVulkanRootSignature* rootSignature = static_cast<FVulkanRootSignature*>(pDesc->pRootSignature);

		for (uint32_t i = 0; i < MAX_SHADER_STAGE; i++)
		{
			if (shader->mShaders[i] != nullptr)
			{
				VkPipelineShaderStageCreateInfo shaderStageInfo = {};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = gShaderStageTranslator[i];
				shaderStageInfo.module = shader->mShaders[i];
				shaderStageInfo.pName = "main";

				shaderStages[numShaderStages] = shaderStageInfo;
				numShaderStages++;
			}
		}

		// Create input state info.
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(rootSignature->mInputBindingDescriptions.size());
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(rootSignature->mInputAttributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = rootSignature->mInputBindingDescriptions.data();
		vertexInputInfo.pVertexAttributeDescriptions = rootSignature->mInputAttributeDescriptions.data();

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
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

		std::vector<VkPushConstantRange> pushConstants(pDesc->numPushConstantRanges);

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
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(rootSignature->mDescriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = rootSignature->mDescriptorSetLayouts.data();

		if (vkCreatePipelineLayout(mDevice->mDevice, &pipelineLayoutInfo, nullptr, &pipeline->mLayout) != VK_SUCCESS) 
		{
			RLOG_ERROR("failed to create pipeline layout!");
			delete pipeline;
			return RR_INITIALIZATION_FAILED;
		}

		FVulkanRenderPass* pass;

		FVulkanRenderPassCreateDesc passDesc;
		passDesc.numColorAttachments = pDesc->numRenderTargets;
		memcpy(&passDesc.colorAttachmentFormats, pDesc->renderTargetFormats, sizeof(passDesc.colorAttachmentFormats));
		passDesc.depthStencilAttachmentFormat = pDesc->depthStencilFormat;
		passDesc.sampleCount = pDesc->sampleCount;
		passDesc.sampleQuality = pDesc->sampleQuality;
		passDesc.pLoadStoreOps = nullptr;

		// Create temporary render pass for this pipeline.
		if (createRenderPass(&pass, &passDesc) != RR_SUCCESS)
		{
			RLOG_ERROR("Failed to create render pass.");
			vkDestroyPipelineLayout(mDevice->mDevice, pipeline->mLayout, nullptr);

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
		pipelineInfo.layout = pipeline->mLayout;
		pipelineInfo.renderPass = pass->mPass;
		pipelineInfo.subpass = 0; 

		if (vkCreateGraphicsPipelines(mDevice->mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->mPipeline) != VK_SUCCESS) 
		{
			RLOG_ERROR("failed to create graphics pipeline!");
			vkDestroyPipelineLayout(mDevice->mDevice, pipeline->mLayout, nullptr);
			destroyRenderPass(pass);

			delete pipeline;
			return RR_INITIALIZATION_FAILED;
		}

		// We don't need the temporary render pass anymore.
		destroyRenderPass(pass);

		pipeline->mRootSignature = rootSignature;

		*ppPipeline = (IPipeline*)pipeline;

		return RR_SUCCESS;
	}


	ERenderResult RVulkanRenderer::createPipeline(IPipeline** ppPipeline, FComputePipelineCreateDesc* pDesc)
	{
		RASSERT(pDesc != nullptr);
		RASSERT(mInitialized);

		RASSERT(pDesc->pShader != nullptr);
		RASSERT(pDesc->pRootSignature != nullptr);

		FVulkanPipeline* pipeline = new FVulkanPipeline();
		pipeline->mType = PIPELINE_TYPE_COMPUTE;

		// Create pipeline shader stage.
		VkPipelineShaderStageCreateInfo shaderStageInfo = {};

		FVulkanShader* shader = static_cast<FVulkanShader*>(pDesc->pShader);
		FVulkanRootSignature* rootSignature = static_cast<FVulkanRootSignature*>(pDesc->pRootSignature);

		if (shader->mShaders[EShaderStage::SHADER_STAGE_COMPUTE] != nullptr)
		{
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			shaderStageInfo.module = shader->mShaders[EShaderStage::SHADER_STAGE_COMPUTE];
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
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(rootSignature->mDescriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = rootSignature->mDescriptorSetLayouts.data();

		if (vkCreatePipelineLayout(mDevice->mDevice, &pipelineLayoutInfo, nullptr, &pipeline->mLayout) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create pipeline layout!");
			delete pipeline;
			return RR_INITIALIZATION_FAILED;
		}

		VkComputePipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.flags = 0;
		pipelineInfo.layout = pipeline->mLayout;
		pipelineInfo.stage = shaderStageInfo;

		if (vkCreateComputePipelines(mDevice->mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->mPipeline) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create graphics pipeline!");
			vkDestroyPipelineLayout(mDevice->mDevice, pipeline->mLayout, nullptr);

			delete pipeline;
			return RR_INITIALIZATION_FAILED;
		}

		// Assign root signature pointer to this pipeline.
		pipeline->mRootSignature = rootSignature;

		*ppPipeline = (IPipeline*)pipeline;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyPipeline(IPipeline* pPipeline)
	{
		RASSERT(pPipeline != nullptr);
		FVulkanPipeline* pipeline = static_cast<FVulkanPipeline*>(pPipeline);
		RASSERT(pipeline->mLayout != nullptr);
		RASSERT(pipeline->mPipeline != nullptr);

		vkDestroyPipeline(mDevice->mDevice, pipeline->mPipeline, nullptr);
		vkDestroyPipelineLayout(mDevice->mDevice, pipeline->mLayout, nullptr);

		delete pPipeline;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createFence(IFence** ppFence, uint64_t value)
	{
		RASSERT(mInitialized);

		FVulkanFence* fence = new FVulkanFence();

		VkFenceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.flags = 0; // We have no need to initialize a fence in a signaled state.

		// Create fence object.
		if (vkCreateFence(mDevice->mDevice, &createInfo, nullptr, &fence->mFence) != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to create a fence object.");
			return RR_INITIALIZATION_FAILED;
			delete fence;
		}

		fence->mSubmitted = false;
		*ppFence = (IFence*)fence;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyFence(IFence* pFence)
	{
		RASSERT(pFence != nullptr);
		FVulkanFence* fence = static_cast<FVulkanFence*>(pFence);

		// Destroy fence object.
		vkDestroyFence(mDevice->mDevice, fence->mFence, nullptr);

		delete pFence;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyFrameBuffer(FVulkanFrameBuffer* pFrameBuffer)
	{
		RASSERT(pFrameBuffer != nullptr);
		
		// Destroy fence object.
		vkDestroyFramebuffer(mDevice->mDevice, pFrameBuffer->mFrameBuffer, nullptr);

		delete pFrameBuffer;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createSemaphore(ISemaphore** ppSemaphore)
	{
		RASSERT(mInitialized);

		FVulkanSemaphore* semaphore = new FVulkanSemaphore();

		VkSemaphoreCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		createInfo.flags = 0; // We have no need to initialize a semaphore in a signaled state.

		// Create semaphore object.
		if (vkCreateSemaphore(mDevice->mDevice, &createInfo, nullptr, &semaphore->mSemaphore) != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to create a semaphore object.");
			return RR_INITIALIZATION_FAILED;
			delete semaphore;
		}

		*ppSemaphore = (ISemaphore*)semaphore;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroySemaphore(ISemaphore* pSemaphore)
	{
		RASSERT(pSemaphore != nullptr);
		FVulkanSemaphore* semaphore = static_cast<FVulkanSemaphore*>(pSemaphore);

		// Destroy semaphore object.
		vkDestroySemaphore(mDevice->mDevice, semaphore->mSemaphore, nullptr);

		delete pSemaphore;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createRenderPass(FVulkanRenderPass** ppRenderPass, FVulkanRenderPassCreateDesc* pDesc)
	{
		RASSERT(pDesc != nullptr);
		RASSERT(mInitialized);

		FVulkanRenderPass* pass = new FVulkanRenderPass();
		pass->mDesc = *pDesc;

		std::vector<VkAttachmentReference> colorAttachmentReferences(pDesc->numColorAttachments);
		std::vector<VkAttachmentDescription> attachments(pDesc->numColorAttachments);

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
					//attachments[numAttachments].stencilLoadOp = gLoadOpTranslator[pDesc->pLoadStoreOps->stencilLoadOp];
					//attachments[numAttachments].stencilStoreOp = gStoreOpTranslator[pDesc->pLoadStoreOps->stencilStoreOp];
				}
				else
				{
					attachments[numAttachments].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
					attachments[numAttachments].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					//attachments[numAttachments].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
					//attachments[numAttachments].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
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
		if (vkCreateRenderPass(mDevice->mDevice, &renderPassInfo, nullptr, &pass->mPass) != VK_SUCCESS)
		{
			delete pass;
			RLOG_ERROR("failed to create render pass!");
			return RR_INITIALIZATION_FAILED;
		}

		*ppRenderPass = pass;
		return RR_SUCCESS;
	}

	void RVulkanRenderer::destroyRenderPass(FVulkanRenderPass* pRenderPass)
	{
		RASSERT(pRenderPass != nullptr);

		vkDestroyRenderPass(mDevice->mDevice, pRenderPass->mPass, nullptr);
		delete pRenderPass;
	}

	ERenderResult RVulkanRenderer::createRootSignature(IRootSignature** ppRootSignature, FRootSignatureCreateDesc* pDesc)
	{
		RASSERT(mInitialized);
		RASSERT(mDevice != nullptr);

		FVulkanRootSignature* rootSignature = new FVulkanRootSignature();

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

		rootSignature->mDescriptorSetLayouts.resize(numSets);

		for (uint32_t set = 0; set < numSets; ++set)
		{
			// Create vector of descriptor set bindings.
			std::vector<VkDescriptorSetLayoutBinding> descriptorSetBindings;

			// Create descriptor set binding layout.
			for (uint32_t i = 0; i < pDesc->numDescriptorBindings; ++i)
			{
				if (pDesc->pDescriptorBindings[i].set != set)
				{
					// Skip this descriptor.
					continue;
				}

				FDescriptorLayoutBinding& descBinding = pDesc->pDescriptorBindings[i];

				VkDescriptorSetLayoutBinding binding;
				binding.binding = descBinding.binding;
				binding.descriptorType = gDescriptorTypeTranslator[descBinding.descriptorType];
				binding.descriptorCount = descBinding.descriptorCount;
				binding.stageFlags = shaderStageFlagsToVkShaderStageFlags(descBinding.stageFlags);

				// Create array of VkSampler objects to bind.
				if (descBinding.descriptorType == DESCRIPTOR_TYPE_SAMPLER)
				{
					FVulkanSampler* sampler = static_cast<FVulkanSampler*>(descBinding.ppImmutableSamplers[0]);
					RASSERT(sampler != nullptr);

					binding.pImmutableSamplers = &sampler->mSampler;
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
			if (vkCreateDescriptorSetLayout(mDevice->mDevice, &layoutInfo, nullptr, &rootSignature->mDescriptorSetLayouts[set]) != VK_SUCCESS)
			{
				RLOG_ERROR("failed to create descriptor set layout!");
				delete rootSignature;
				return RR_INITIALIZATION_FAILED;
			}
		}

		rootSignature->mInputAttributeDescriptions.resize(pDesc->pInputAssemblerLayout->numInputAttributeDescriptions);
		rootSignature->mInputBindingDescriptions.resize(pDesc->pInputAssemblerLayout->numInputBindingDescriptions);

		// Create input attribute descriptions.
		for (uint32_t i = 0; i < pDesc->pInputAssemblerLayout->numInputAttributeDescriptions; ++i)
		{
			FInputAttributeDescription* attribute = &pDesc->pInputAssemblerLayout->pInputAttributeDescriptions[i];
			rootSignature->mInputAttributeDescriptions[i].binding = attribute->binding;
			rootSignature->mInputAttributeDescriptions[i].format = gTextureFormatTranslator[attribute->format];
			rootSignature->mInputAttributeDescriptions[i].location = attribute->location;
			rootSignature->mInputAttributeDescriptions[i].offset = attribute->offset;
		}

		// Create input binding descriptions.
		for (uint32_t i = 0; i < pDesc->pInputAssemblerLayout->numInputBindingDescriptions; ++i)
		{
			FInputBindingDescription* binding = &pDesc->pInputAssemblerLayout->pInputBindingDescriptions[i];
			rootSignature->mInputBindingDescriptions[i].binding = binding->binding;
			rootSignature->mInputBindingDescriptions[i].inputRate = gInputRateTranslator[binding->inputRate];
			rootSignature->mInputBindingDescriptions[i].stride = binding->stride;
		}

		// Assign shader to this root signature object.
		rootSignature->mpShader = static_cast<FVulkanShader*>(pDesc->pShaderProgram);

		*ppRootSignature = static_cast<IRootSignature*>(rootSignature);
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyRootSignature(IRootSignature* pRootSignature)
	{
		RASSERT(pRootSignature != nullptr);
		
		FVulkanRootSignature* rootSignature = static_cast<FVulkanRootSignature*>(pRootSignature);

		for (uint32_t i = 0; i < rootSignature->mDescriptorSetLayouts.size(); ++i)
		{
			vkDestroyDescriptorSetLayout(mDevice->mDevice, rootSignature->mDescriptorSetLayouts[i], nullptr);
		}

		delete pRootSignature;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createShader(IShader** ppShader, FShaderCreateDesc* pDesc)
	{
		RASSERT(mInitialized);
		RASSERT(mDevice != nullptr);
		
		FVulkanShader* shader = new FVulkanShader();
		memset(shader->mShaders, 0, sizeof(shader->mShaders));

		for (uint32_t i = 0; i < MAX_SHADER_STAGE; ++i)
		{
			if (!pDesc->shaders[i].path.empty())
			{
				if (shader->mShaders[pDesc->shaders[i].stage] != nullptr)
				{
					RLOG_ERROR("Trying to create a shader program which has multiple shaders for the same stage is incorrect.");
					return RR_ALREADY_INITIALIZED;
				}

				std::vector<char> shaderCode = RPlatformFileSystem::ReadFile(pDesc->shaders[i].path);

				VkShaderModuleCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = shaderCode.size();
				createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

				if (vkCreateShaderModule(mDevice->mDevice, &createInfo, nullptr, &shader->mShaders[pDesc->shaders[i].stage]) != VK_SUCCESS)
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

	ERenderResult RVulkanRenderer::destroyShader(IShader* pShader)
	{
		RASSERT(mInitialized);
		RASSERT(mDevice != nullptr);
		RASSERT(pShader != nullptr);

		FVulkanShader* shader = static_cast<FVulkanShader*>(pShader);

		for (uint32_t i = 0; i < MAX_SHADER_STAGE; ++i)
		{
			if (shader->mShaders[i] != nullptr)
			{
				vkDestroyShaderModule(mDevice->mDevice, shader->mShaders[i], nullptr);
			}
		}

		delete shader;
		return RR_SUCCESS;
	}
	
	ERenderResult RVulkanRenderer::createSampler(ISampler** ppSampler, FSamplerCreateDesc* pDesc)
	{
		RASSERT(mInitialized);
		RASSERT(mDevice != nullptr);
		RASSERT(pDesc != nullptr);

		FVulkanSampler* sampler = new FVulkanSampler();

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.addressModeU = gSamplerAddressModeTranslator[pDesc->addressModeU];
		samplerInfo.addressModeV = gSamplerAddressModeTranslator[pDesc->addressModeV];
		samplerInfo.addressModeW = gSamplerAddressModeTranslator[pDesc->addressModeW];
		samplerInfo.anisotropyEnable = pDesc->anisotropyEnable;
		samplerInfo.borderColor = gBorderColorTranslator[pDesc->borderColor];
		samplerInfo.compareEnable = pDesc->comparisonFunc != ESamplerComparisonFunc::SAMPLER_COMPARISON_NONE;
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

		// Create image object.
		if (vkCreateSampler(mDevice->mDevice, &samplerInfo, nullptr, &sampler->mSampler) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create sampler!");
			delete sampler;
			return RR_INITIALIZATION_FAILED;
		}

		// Assign sampler description.
		sampler->mDesc = *pDesc;

		// Set the image description of the texture to the one of the created image.
		*ppSampler = sampler;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroySampler(ISampler* pSampler)
	{
		RASSERT(pSampler != nullptr);

		FVulkanSampler* sampler = static_cast<FVulkanSampler*>(pSampler);
		vkDestroySampler(mDevice->mDevice, sampler->mSampler, nullptr);

		delete pSampler;
		return RR_SUCCESS;
	}

	uint32_t RVulkanRenderer::findSuitableMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(mDevice->mPhysicalDevice->mPhysicalDevice, &memProperties);

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

	ERenderResult RVulkanRenderer::createTexture(ITexture** ppTexture, FTextureCreateDesc* pDesc)
	{
		RASSERT(pDesc != nullptr);
		RASSERT(pDesc->arrayLayers > 0);

		FVulkanTexture* texture = new FVulkanTexture();
		
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = gImageTypeTranslator[pDesc->type];
		imageInfo.extent.width = pDesc->width;
		imageInfo.extent.height = pDesc->height;
		imageInfo.extent.depth = pDesc->depth;
		imageInfo.mipLevels = pDesc->mipLevels;
		imageInfo.arrayLayers = pDesc->type == TEXTURE_TYPE_CUBE ? pDesc->arrayLayers * 6 : pDesc->arrayLayers;
		imageInfo.format = gTextureFormatTranslator[pDesc->format];
		imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = textureUsageFlagsToVkImageFlags(pDesc->usage);
		imageInfo.samples = textureSamplesToVkSamples(pDesc->sampleCount);
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (pDesc->nativeSwapchainImage == nullptr)
		{		
			// Create image object.
			if (vkCreateImage(mDevice->mDevice, &imageInfo, nullptr, &texture->mImage) != VK_SUCCESS)
			{
				RLOG_ERROR("failed to create image!");
				delete texture;
				return RR_INITIALIZATION_FAILED;
			}
		}
		else
		{
			texture->mImage = (VkImage)pDesc->nativeSwapchainImage;
		}


		// Not thread-safe.
		texture->mTextureId = ++gMaxTextureId;

		// Set the image description of the texture to the one of the created image.
		texture->mDesc = *pDesc;
		texture->mVkImageDesc = imageInfo;
		texture->mState = FResourceState::RESOURCE_STATE_UNDEFINED;

		if (pDesc->nativeSwapchainImage == nullptr)
		{
			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = gMemoryTypeTranslator[pDesc->memoryType];

			vmaAllocateMemoryForImage(mAllocator, texture->mImage, &allocInfo, &texture->mAllocation, nullptr);
			vmaBindImageMemory(mAllocator, texture->mAllocation, texture->mImage);
		}

		VkImageViewType viewType;

		switch (pDesc->type)
		{
			case TEXTURE_TYPE_1D:
			{
				viewType = pDesc->arrayLayers <= 1 ? VK_IMAGE_VIEW_TYPE_1D : VK_IMAGE_VIEW_TYPE_1D_ARRAY;
			}
			break;
			case TEXTURE_TYPE_2D:
			{
				viewType = pDesc->arrayLayers <= 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			}
			break;
			case TEXTURE_TYPE_3D:
			{
				RASSERT(pDesc->arrayLayers == 1);
				viewType = VK_IMAGE_VIEW_TYPE_3D;
			}
			break;
			case TEXTURE_TYPE_CUBE:
			{
				RASSERT(pDesc->arrayLayers >= 6);
				RASSERT(pDesc->arrayLayers % 6 == 0);

				viewType = pDesc->arrayLayers == 6 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
			}
			break;
			default:
			{
				viewType = VK_IMAGE_VIEW_TYPE_2D;
				RLOG_ERROR("We received an unknown texture type while creating a texture.");
			}
			break;
		}

		// Create image view.
		texture->mImageView = createImageView(texture, viewType, imageInfo.format, imageAspectFlagFromFormat(imageInfo.format), texture->mVkImageViewDesc);

		// Copy initial data inside the buffer.
		if (texture->mDesc.pData != nullptr)
		{
			//create a staging buffer.
			FVulkanBuffer* stagingBuffer = nullptr;

			FBufferCreateDesc stagingCreateDesc = {};
			stagingCreateDesc.memoryType = MEMORY_TYPE_CPU;
			stagingCreateDesc.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;
			stagingCreateDesc.data = nullptr;
			stagingCreateDesc.size = (uint32_t)texture->mAllocation->GetSize();

			if (createBuffer((IBuffer**)&stagingBuffer, &stagingCreateDesc) != RR_SUCCESS)
			{
				RLOG_WARNING("Failed to allocate a staging buffer, therefore the newly created buffer won't have any contents.");
			}

			// Copy data into staging buffer.
			void* data;
			vmaMapMemory(mAllocator, stagingBuffer->mAllocation, &data);
			memcpy(data, pDesc->pData, texture->mAllocation->GetSize());
			vmaUnmapMemory(mAllocator, stagingBuffer->mAllocation);

			FVulkanFence* pTransferCompleteFence = nullptr;
			FVulkanCommandList* pTransferCmd = nullptr;
			createCommandList((ICommandList**)&pTransferCmd, mTransferCommandPool);
			createFence((IFence**)&pTransferCompleteFence);

			// copy buffer.
			cmdBegin(pTransferCmd);
			cmdCopyBufferToTexture(pTransferCmd, stagingBuffer, texture, 0, pDesc->width, pDesc->height);
			cmdEnd(pTransferCmd);

			// Submit command 
			queueSubmit(mTransferQueue, 1, (ICommandList**)&pTransferCmd, pTransferCompleteFence, 0, nullptr, 0, nullptr);

			// Wait for transfer to complete.
			waitForFence(pTransferCompleteFence);
			destroyFence(pTransferCompleteFence);
			destroyCommandList(mTransferCommandPool, pTransferCmd);
			destroyBuffer(stagingBuffer);
		}

		*ppTexture = texture;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyTexture(ITexture* pTexture)
	{
		RASSERT(pTexture != nullptr);

		FVulkanTexture* texture = static_cast<FVulkanTexture*>(pTexture);

		vkDestroyImageView(mDevice->mDevice, texture->mImageView, nullptr);
		vkDestroyImage(mDevice->mDevice, texture->mImage, nullptr);
		vmaFreeMemory(mAllocator, texture->mAllocation);

		delete pTexture;

		return RR_SUCCESS;
	}


	ERenderResult RVulkanRenderer::createBuffer(IBuffer** ppBuffer, FBufferCreateDesc* pDesc)
	{
		RASSERT(pDesc != nullptr);

		FVulkanBuffer* buffer = new FVulkanBuffer();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = pDesc->size;
		bufferInfo.usage = bufferUsageFlagsToVkBufferFlags(pDesc->usage);
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = gMemoryTypeTranslator[pDesc->memoryType];

		// Create image object.
		if (vmaCreateBuffer(mAllocator, &bufferInfo, &allocInfo, &buffer->mBuffer, &buffer->mAllocation, nullptr) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create buffer!");
			delete buffer;
			return RR_INITIALIZATION_FAILED;
		}

		// Set the image description of the texture to the one of the created image.
		buffer->mDesc = *pDesc;
		buffer->mState = FResourceState::RESOURCE_STATE_COMMON;
		
		// Copy initial data inside the buffer.
		if (buffer->mDesc.data != nullptr)
		{
			// If we have coherent memory, copy directly into the memory.
			if (buffer->mDesc.memoryType == EMemoryType::MEMORY_TYPE_CPU)
			{
				void* data;
				VkResult res = vmaMapMemory(mAllocator, buffer->mAllocation, &data);
				memcpy(data, pDesc->data, pDesc->size);
				vmaUnmapMemory(mAllocator, buffer->mAllocation);
			}
			else
			{
				//create a staging buffer.
				FVulkanBuffer* stagingBuffer = nullptr;

				FBufferCreateDesc stagingCreateDesc = {};
				stagingCreateDesc.memoryType = MEMORY_TYPE_CPU;
				stagingCreateDesc.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;
				stagingCreateDesc.data = nullptr;
				stagingCreateDesc.size = pDesc->size;

				if (createBuffer((IBuffer**)&stagingBuffer, &stagingCreateDesc) != RR_SUCCESS)
				{
					RLOG_WARNING("Failed to allocate a staging buffer, therefore the newly created buffer won't have any contents.");
				}

				// Copy data into staging buffer.
				void* data;
				vmaMapMemory(mAllocator, stagingBuffer->mAllocation, &data);
				memcpy(data, pDesc->data, pDesc->size);
				vmaUnmapMemory(mAllocator, stagingBuffer->mAllocation);

				FVulkanFence* pTransferCompleteFence = nullptr;
				FVulkanCommandList* pTransferCmd = nullptr;
				createCommandList((ICommandList**)&pTransferCmd, mTransferCommandPool);
				createFence((IFence**)&pTransferCompleteFence);

				// copy buffer.
				cmdBegin(pTransferCmd);
				cmdCopyBuffer(pTransferCmd, stagingBuffer, buffer, 0, 0, static_cast<uint32_t>(pDesc->size));
				cmdEnd(pTransferCmd);

				// Submit command 
				queueSubmit(mTransferQueue, 1, (ICommandList**)&pTransferCmd, pTransferCompleteFence, 0, nullptr, 0, nullptr);

				// Wait for transfer to complete.
				waitForFence(pTransferCompleteFence);
				destroyFence(pTransferCompleteFence);
				destroyCommandList(mTransferCommandPool, pTransferCmd);
				destroyBuffer(stagingBuffer);
			}
		}

		*ppBuffer = (IBuffer*)buffer;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyBuffer(IBuffer* pBuffer)
	{
		RASSERT(pBuffer != nullptr);

		FVulkanBuffer* buffer = static_cast<FVulkanBuffer*>(pBuffer);
		vmaDestroyBuffer(mAllocator, buffer->mBuffer, buffer->mAllocation);

		delete pBuffer;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::cmdCopyBuffer(ICommandList* pCommandList, IBuffer* pSrcBuffer, IBuffer* pDstBuffer, uint32_t srcOffset, uint32_t dstOffset, uint32_t size)
	{
		RASSERT(pCommandList != nullptr);
		RASSERT(pSrcBuffer != nullptr);
		RASSERT(pDstBuffer != nullptr);

		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);
		FVulkanBuffer* srcBuffer = static_cast<FVulkanBuffer*>(pSrcBuffer);
		FVulkanBuffer* dstBuffer = static_cast<FVulkanBuffer*>(pDstBuffer);

		FBufferBarrier bBarriers[2];

		bBarriers[0].buffer = pSrcBuffer;
		bBarriers[0].requiredState = FResourceState::RESOURCE_STATE_COPY_SOURCE;

		bBarriers[1].buffer = pDstBuffer;
		bBarriers[1].requiredState = FResourceState::RESOURCE_STATE_COPY_DEST;

		cmdResourceBarriers(pCommandList, 2, &bBarriers[0], 0, nullptr, true);

		// Set copy region.
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = srcOffset; // Optional
		copyRegion.dstOffset = dstOffset; // Optional
		copyRegion.size = size;

		// Copy the buffer.
		vkCmdCopyBuffer(list->mCmdBuffer, srcBuffer->mBuffer, dstBuffer->mBuffer, 1, &copyRegion);

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::cmdCopyBufferToTexture(ICommandList* pCommandList, IBuffer* pSrcBuffer, ITexture* pDstImage, uint32_t srcOffset, uint32_t width, uint32_t height)
	{
		FBufferBarrier bBarrier;
		bBarrier.buffer = pSrcBuffer;
		bBarrier.requiredState = FResourceState::RESOURCE_STATE_COPY_SOURCE;

		FTextureBarrier tBarrier;
		tBarrier.texture = pDstImage;
		tBarrier.requiredState = FResourceState::RESOURCE_STATE_COPY_DEST;

		cmdResourceBarriers(pCommandList, 1, &bBarrier, 1, &tBarrier, true);

		RASSERT(pCommandList != nullptr);
		RASSERT(pSrcBuffer != nullptr);
		RASSERT(pDstImage != nullptr);

		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);
		FVulkanBuffer* srcBuffer = static_cast<FVulkanBuffer*>(pSrcBuffer);
		FVulkanTexture* dstImage = static_cast<FVulkanTexture*>(pDstImage);

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
			list->mCmdBuffer,
			srcBuffer->mBuffer,
			dstImage->mImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		FTextureBarrier tEndBarrier;
		tEndBarrier.texture = pDstImage;
		tEndBarrier.requiredState = FResourceState::RESOURCE_STATE_SHADER_RESOURCE;

		cmdResourceBarriers(pCommandList, 0, nullptr, 1, &tEndBarrier, true);

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::cmdUpdateBuffer(ICommandList* pCommandList, IBuffer* pDstBuffer, void* pData, uint32_t dstOffset, uint32_t size)
	{
		RASSERT(pData != nullptr);
		RASSERT(pCommandList != nullptr);
		RASSERT(pDstBuffer != nullptr);
		RASSERT(size != 0);

		FVulkanBuffer* buffer = static_cast<FVulkanBuffer*>(pDstBuffer);
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		if (buffer->mDesc.memoryType == MEMORY_TYPE_CPU)
		{
			void* data;
			vmaMapMemory(mAllocator, buffer->mAllocation, &data);
			memcpy(data, pData, size);
			vmaUnmapMemory(mAllocator, buffer->mAllocation);

			return RR_SUCCESS;
		}
		else
		{
			//create a staging buffer.
			FVulkanBuffer* stagingBuffer = nullptr;

			FBufferCreateDesc stagingCreateDesc = {};
			stagingCreateDesc.memoryType = MEMORY_TYPE_CPU;
			stagingCreateDesc.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;
			stagingCreateDesc.data = nullptr;
			stagingCreateDesc.size = size;

			if (createBuffer((IBuffer**)&stagingBuffer, &stagingCreateDesc) != RR_SUCCESS)
			{
				RLOG_ERROR("Failed to allocate a staging buffer, therefore the newly created buffer won't have any contents.");
				return RR_FAIL;
			}

			// Copy data into staging buffer.
			void* data;
			vmaMapMemory(mAllocator, stagingBuffer->mAllocation, &data);
			memcpy(data, pData, size);
			vmaUnmapMemory(mAllocator, stagingBuffer->mAllocation);

			// copy buffer.
			cmdCopyBuffer(list, stagingBuffer, buffer, 0, dstOffset, size);

			destroyBuffer(stagingBuffer);

			return RR_SUCCESS;
		}
	}

	ERenderResult RVulkanRenderer::cmdUpdateTexture(ICommandList* pCommandList, ITexture* pDstTexture, void* pData, uint32_t width, uint32_t height)
	{
		RASSERT(pData != nullptr);
		RASSERT(pCommandList != nullptr);
		RASSERT(pDstTexture != nullptr);
		RASSERT(!(width == 0 && height == 0));

		FVulkanTexture* texture = static_cast<FVulkanTexture*>(pDstTexture);
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);
	
		//create a staging buffer.
		FVulkanBuffer* stagingBuffer = nullptr;

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(mDevice->mDevice, texture->mImage, &memRequirements);

		FBufferCreateDesc stagingCreateDesc = {};
		stagingCreateDesc.memoryType = MEMORY_TYPE_CPU;
		stagingCreateDesc.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;
		stagingCreateDesc.data = nullptr;
		stagingCreateDesc.size = memRequirements.size;

		if (createBuffer((IBuffer**)&stagingBuffer, &stagingCreateDesc) != RR_SUCCESS)
		{
			RLOG_WARNING("Failed to allocate a staging buffer, therefore the newly created buffer won't have any contents.");
		}

		// Copy data into staging buffer.
		void* data;
		vmaMapMemory(mAllocator, stagingBuffer->mAllocation, &data);
		memcpy(data, pData, memRequirements.size);
		vmaUnmapMemory(mAllocator, stagingBuffer->mAllocation);

		// copy buffer.
		cmdCopyBufferToTexture(list, stagingBuffer, texture, 0, width, height);

		destroyBuffer(stagingBuffer);

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createRenderTarget(IRenderTarget** ppRenderTarget, FRenderTargetCreateDesc* pDesc)
	{
		RASSERT(pDesc != nullptr);

		FVulkanRenderTarget* renderTarget = new FVulkanRenderTarget();

		FTextureUsageFlags usage = TEXTURE_USAGE_TRANSFER_SRC | TEXTURE_USAGE_TRANSFER_DST | TEXTURE_USAGE_SAMPLED;
		if ((pDesc->usage & FRenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT) != 0)
		{
			usage |= FTextureUsageFlags::TEXTURE_USAGE_COLOR_ATTACHMENT;
		}

		if ((pDesc->usage & FRenderTargetUsage::RENDER_TARGET_USAGE_DEPTH_STENCIL_ATTACHMENT) != 0)
		{
			usage |= FTextureUsageFlags::TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT;
		}

		FTextureCreateDesc createDesc = {};
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
		createDesc.type = ETextureType::TEXTURE_TYPE_2D;
		createDesc.usage = usage;
		createDesc.nativeSwapchainImage = pDesc->nativeSwapchainImage;
		createDesc.pData = nullptr;

		// Create a texture for this render target.
		if (createTexture((ITexture**)&renderTarget->mTexture, &createDesc) != RR_SUCCESS)
		{
			RLOG_ERROR("Failed to create a texture for this render target.");
			return RR_INITIALIZATION_FAILED;
		}

		renderTarget->mDesc = *pDesc;

		*ppRenderTarget = (FVulkanRenderTarget*)renderTarget;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyRenderTarget(IRenderTarget* pRenderTarget)
	{
		RASSERT(pRenderTarget != nullptr);

		FVulkanRenderTarget* target = static_cast<FVulkanRenderTarget*>(pRenderTarget);
		destroyTexture(target->mTexture);

		delete pRenderTarget;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::setupDebugCallback()
	{
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT; // only errors and warnings.
		createInfo.pfnCallback = debugCallback;

		// Create debug extension.
		if (createDebugReportCallbackExtension(mInstance, &createInfo, nullptr, &mDebuggingCallback) != VK_SUCCESS) {
			return RR_INITIALIZATION_FAILED;
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::queryAvailablePhysicalDevices()
	{
		// Query how many physical devices we have.
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

		// We don't have any gpu's with vulkan support so we can't proceed.
		if (deviceCount == 0) {
			return RR_NO_SUITABLE_DEVICE;
		}

		// Resize the gpus vector to hold all physical devices.
		mGPUs.resize(deviceCount);

		// Get the list of the physical devices we have.
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

		for (uint32_t i = 0; i < deviceCount; i++)
		{
			mGPUs[i] = FVulkanPhysicalDevice();

			// Fill our physical device structures.
			mGPUs[i].mPhysicalDevice = devices[i];

			// Query device properties.
			vkGetPhysicalDeviceProperties(devices[i], &mGPUs[i].mProperties);

			// Query device memory properties.
			vkGetPhysicalDeviceMemoryProperties(devices[i], &mGPUs[i].mMemoryProperties);

			// Query device features.
			vkGetPhysicalDeviceFeatures(devices[i], &mGPUs[i].mFeatures);

			// Get amount of available queue families.
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);

			// Get the list of queue families.
			mGPUs[i].mQueueFamilyProperties.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, mGPUs[i].mQueueFamilyProperties.data());

			// query the amount of extensions this physical device supports.
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);

			// Retrieve the extension list.
			mGPUs[i].mSupportedExtensions.resize(extensionCount);
			vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, mGPUs[i].mSupportedExtensions.data());
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::pickPhysicalDevice(FVulkanPhysicalDevice** ppDevice)
	{
		// Safety check to make sure we have retrieved all information about the physical devices.
		if (mGPUs.empty())
		{
			ERenderResult result = queryAvailablePhysicalDevices();
			if (result != RR_SUCCESS)
			{
				return result;
			}
		}

		// We require swapchain support for our device.
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		for (auto extension : mRendererInfo.requiredVulkanDeviceExtensions)
		{
			deviceExtensions.push_back(extension.c_str());
		}

		// Find the most suitable physical device.
		FVulkanPhysicalDevice* mostSuitableDevice = nullptr;
		uint32_t deviceScore = 0;

		for (auto& candidate : mGPUs)
		{
			// Calculate score for this device.
			uint32_t score = calculatePhysicalDeviceScore(&candidate, deviceExtensions);

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

	FQueueFamilyIndices RVulkanRenderer::retrieveQueueFamilyIndices(const FVulkanPhysicalDevice* pDevice)
	{
		FQueueFamilyIndices indices;
		indices.mGraphicsQueueIndex = retrieveQueueFamilyIndex(pDevice, VK_QUEUE_GRAPHICS_BIT);
		indices.mComputeQueueIndex = retrieveQueueFamilyIndex(pDevice, VK_QUEUE_COMPUTE_BIT);
		indices.mTransferQueueIndex = retrieveQueueFamilyIndex(pDevice, VK_QUEUE_TRANSFER_BIT);

		return indices;
	}

	uint32_t RVulkanRenderer::retrieveQueueFamilyIndex(const FVulkanPhysicalDevice* pDevice, VkQueueFlags flags)
	{
		// Try to find a dedicated compute queue.
		if (flags & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < pDevice->mQueueFamilyProperties.size(); i++)
			{
				// If the queue supports compute and does NOT support graphics, we can return this as a dedicated compute queue.
				if (pDevice->mQueueFamilyProperties[i].queueCount > 0 &&
					(pDevice->mQueueFamilyProperties[i].queueFlags & flags)
					&& ((pDevice->mQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
					return i;
				}
			}
		}

		// Try to find a dedicated transfer queue.
		if (flags & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < pDevice->mQueueFamilyProperties.size(); i++)
			{
				// If the queue supports compute and does NOT support graphics or compute, we can return this as a dedicated compute queue.
				if (pDevice->mQueueFamilyProperties[i].queueCount > 0 &&
					(pDevice->mQueueFamilyProperties[i].queueFlags & flags) &&
					((pDevice->mQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
					((pDevice->mQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					return i;
				}
			}
		}

		// Otherwise return the first queue which matches the flags.
		for (uint32_t i = 0; i < pDevice->mQueueFamilyProperties.size(); i++)
		{
			// If the queue supports compute and does NOT support graphics or compute, we can return this as a dedicated compute queue.
			if (pDevice->mQueueFamilyProperties[i].queueCount > 0 &&
				(pDevice->mQueueFamilyProperties[i].queueFlags & flags)) {
				return i;
			}
		}

		// We couldn't find a queue which supports the specified flags so we return an invalid queue index.
		return UINT32_MAX;
	}

	uint32_t RVulkanRenderer::calculatePhysicalDeviceScore(const FVulkanPhysicalDevice* pDevice, const std::vector<const char*>& rRequiredDeviceExtensions)
	{
		uint32_t score = 0;

		// We require device extensions to be available.
		if (!hasDeviceExtensionSupport(pDevice, rRequiredDeviceExtensions))
		{
			// We don't have required device extensions therefore this device is unsuitable.
			return 0;
		}

		// Retrieve queue family indices.
		FQueueFamilyIndices queueFamilyIndices = retrieveQueueFamilyIndices(pDevice);

		// A dedicated GPU is preferred.
		if (pDevice->mProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		// If we have dedicated queues for transferral and compute, our score goes up.
		if (queueFamilyIndices.hasDedicatedComputeQueue())
		{
			score += 100;
		}

		if (queueFamilyIndices.hasDedicatedTransferQueue())
		{
			score += 100;
		}

		if (pDevice->mFeatures.tessellationShader)
		{
			score += 100;
		}

		if (pDevice->mFeatures.geometryShader)
		{
			score += 100;
		}

		return score;
	}

	ERenderResult RVulkanRenderer::createLogicalDevice(FVulkanPhysicalDevice* pPhysicalDevice)
	{
		mDevice = new FVulkanDevice();

		// Find queue indices.
		mDevice->mQueueIndices = retrieveQueueFamilyIndices(pPhysicalDevice);

		// Vector of queue creation information structures.
		std::vector<VkDeviceQueueCreateInfo> queueInfos;

		std::vector<std::vector<float>>queuePriorities(pPhysicalDevice->mQueueFamilyProperties.size());

		for (uint32_t i = 0; i < pPhysicalDevice->mQueueFamilyProperties.size(); ++i)
		{
			queuePriorities[i].resize(pPhysicalDevice->mQueueFamilyProperties[i].queueCount);
			for (uint32_t j = 0; j < pPhysicalDevice->mQueueFamilyProperties[i].queueCount; ++j)
			{
				queuePriorities[i][j] = 1.0f;
			}

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = i;
			queueCreateInfo.queueCount = pPhysicalDevice->mQueueFamilyProperties[i].queueCount;
			queueCreateInfo.pQueuePriorities = queuePriorities[i].data();
			queueInfos.push_back(queueCreateInfo);
		}

		// We want certain device features to be enabled.
		mDevice->mEnabledFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
		createInfo.pEnabledFeatures = &mDevice->mEnabledFeatures;

		createInfo.enabledExtensionCount = 0;

		// Enable the same validation layers for this logical device.
		std::vector<const char*> validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		for (auto layer : mRendererInfo.requiredVulkanDeviceLayers)
		{
			validationLayers.push_back(layer.c_str());
		}

		// Enable swapchain extensions.
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		for (auto extension : mRendererInfo.requiredVulkanDeviceExtensions)
		{
			deviceExtensions.push_back(extension.c_str());
		}

		if (mRendererInfo.enableDebugging) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Create the device.
		if (vkCreateDevice(pPhysicalDevice->mPhysicalDevice, &createInfo, nullptr, &mDevice->mDevice) != VK_SUCCESS) 
		{
			delete mDevice;
			mDevice = nullptr;

			return RR_INITIALIZATION_FAILED;
		}

		mDevice->mPhysicalDevice = pPhysicalDevice;
		return RR_SUCCESS;
	}

	bool RVulkanRenderer::hasDeviceExtensionSupport(const FVulkanPhysicalDevice* pDevice, const std::vector<const char*>& rDeviceExtensions)
	{
		// Go over the extensions and remove them if we support it. If the list is empty at the end, it means we support all extensions.
		std::set<std::string> requiredExtensions(rDeviceExtensions.begin(), rDeviceExtensions.end());

		for (const auto& extension : pDevice->mSupportedExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	VkImageView RVulkanRenderer::createImageView(FVulkanTexture* pImage, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewCreateInfo& desc)
	{
		desc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		desc.image = pImage->mImage;
		desc.viewType = viewType;
		desc.format = format;
		desc.subresourceRange.aspectMask = aspectFlags;
		desc.subresourceRange.baseMipLevel = pImage->mDesc.baseMipLevel;
		desc.subresourceRange.levelCount = pImage->mDesc.mipLevels;
		desc.subresourceRange.baseArrayLayer = pImage->mDesc.baseArrayLayer;
		desc.subresourceRange.layerCount = pImage->mDesc.arrayLayers * (pImage->mDesc.type == TEXTURE_TYPE_CUBE ? 6 : 1);
		desc.components.a = VK_COMPONENT_SWIZZLE_A;
		desc.components.r = VK_COMPONENT_SWIZZLE_R;
		desc.components.g = VK_COMPONENT_SWIZZLE_G;
		desc.components.b = VK_COMPONENT_SWIZZLE_B;

		VkImageView imageView;
		if (vkCreateImageView(mDevice->mDevice, &desc, nullptr, &imageView) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create texture image view!");
			return nullptr;
		}

		return imageView;
	}

	ERenderResult RVulkanRenderer::destroy()
	{
		// If the renderer is not initialized, we have nothing to destroy; thefore we immediately return success.
		if (!isInitialized())
		{
			return RR_SUCCESS;
		}

		if(mRendererInfo.enableDebugging)
		{
			// Destroy debugging report callback.
			destroyDebugReportCallbackExtention(mInstance, mDebuggingCallback, nullptr);
		}

		// Destroy our global descriptor pool.
		destroyDescriptorPool(mDescriptorPool);

		// Destroy transfer queue and pool.
		destroyCommandPool(mTransferCommandPool);
		destroyQueue(mTransferQueue);

		for (auto it = mFrameBufferMap.begin(); it != mFrameBufferMap.end(); ++it)
		{
			destroyFrameBuffer(it->second);
		}

		for (auto it = mRenderPassMap.begin(); it != mRenderPassMap.end(); ++it)
		{
			destroyRenderPass(it->second);
		}

		mFrameBufferMap.clear();
		mRenderPassMap.clear();

		vmaDestroyAllocator(mAllocator);

		// Destroy our logical device.
		vkDestroyDevice(mDevice->mDevice, nullptr);

		// Destroy dynamically allocated device wrapper.
		delete mDevice;
		mDevice = nullptr;

		// Destroy our Vulkan instance.
		vkDestroyInstance(mInstance, nullptr);

		return RR_SUCCESS;
	}
	
	ERenderResult RVulkanRenderer::createDescriptorPool(FVulkanDescriptorPool** ppDescriptorPool, FVulkanDescriptorPoolCreateDesc* pDesc)
	{
		RASSERT(pDesc != nullptr);
		RASSERT(mDevice != nullptr);

		FVulkanDescriptorPool* pool = new FVulkanDescriptorPool();

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = pDesc->numPoolSizes;
		poolInfo.pPoolSizes = pDesc->pPoolSizes;
		poolInfo.maxSets = pDesc->maxDescriptorSets;
		poolInfo.flags = pDesc->flags;

		if (vkCreateDescriptorPool(mDevice->mDevice, &poolInfo, nullptr, &pool->mDescriptorPool) != VK_SUCCESS) 
		{
			RLOG_ERROR("failed to create descriptor pool!");
			delete pool;

			return RR_INITIALIZATION_FAILED;
		}

		pool->mFlags = pDesc->flags;
		pool->mMaxDescriptorSets = pDesc->maxDescriptorSets;
		pool->mNumDescriptorSets = 0;

		*ppDescriptorPool = pool;

		return RR_SUCCESS;
	}

	void RVulkanRenderer::destroyDescriptorPool(FVulkanDescriptorPool* pDescriptorPool)
	{
		RASSERT(pDescriptorPool != nullptr);
		vkDestroyDescriptorPool(mDevice->mDevice, pDescriptorPool->mDescriptorPool, nullptr);
		delete pDescriptorPool;
	}

	ERenderResult RVulkanRenderer::queueSubmit(IQueue* pQueue, uint32_t numCmds, ICommandList** ppCmds, IFence* pSignalFence, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores, uint32_t numSignalSemaphores, ISemaphore** ppSignalSemaphores)
	{
		RASSERT(pQueue != nullptr);
		RASSERT(numCmds != 0);

		FVulkanQueue* queue = static_cast<FVulkanQueue*>(pQueue);
		FVulkanFence* fence = static_cast<FVulkanFence*>(pSignalFence);

		std::vector<VkCommandBuffer> commandBuffers(numCmds);
		std::vector<VkSemaphore> waitSemaphores(numWaitSemaphores);
		std::vector<VkPipelineStageFlags> waitSemaphoreDestMasks(numWaitSemaphores);
		std::vector<VkSemaphore> signalSemaphores(numSignalSemaphores);

		// Retrieve vulkan objects from wrapper objects.
		for (uint32_t i = 0; i < numCmds; ++i)
		{
			FVulkanCommandList* cmdList = static_cast<FVulkanCommandList*>(ppCmds[i]);
			RASSERT(cmdList != nullptr);

			commandBuffers[i] = cmdList->mCmdBuffer;
		}

		for (uint32_t i = 0; i < numWaitSemaphores; ++i)
		{
			FVulkanSemaphore* semaphore = static_cast<FVulkanSemaphore*>(ppWaitSemaphores[i]);
			RASSERT(semaphore != nullptr);

			waitSemaphores[i] = semaphore->mSemaphore;
			waitSemaphoreDestMasks[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}

		for (uint32_t i = 0; i < numSignalSemaphores; ++i)
		{
			FVulkanSemaphore* semaphore = static_cast<FVulkanSemaphore*>(ppSignalSemaphores[i]);
			RASSERT(semaphore != nullptr);

			signalSemaphores[i] = semaphore->mSemaphore;
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
			fence->mSubmitted = true;
		}

		VkResult res = vkQueueSubmit(queue->mQueue, 1, &submitInfo, fence != nullptr ? fence->mFence : nullptr);
		// Submit the commands to the queue.
		if (res != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to submit command lists to the specified command queue.");
			return RR_UNKNOWN_FAILURE;
		}

		// We successfully submitted our commands to the queue.
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::waitForQueue(IQueue* pQueue)
	{
		RASSERT(pQueue != nullptr);

		// Get vulkan wrapper object of the queue.
		FVulkanQueue* queue = static_cast<FVulkanQueue*>(pQueue);

		vkQueueWaitIdle(queue->mQueue);
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::waitForDevice()
	{
		vkDeviceWaitIdle(mDevice->mDevice);
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::queuePresent(IQueue* pQueue, uint32_t numSwapChains, ISwapChain** ppSwapChains, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores)
	{
		RASSERT(pQueue != nullptr);
		RASSERT(numSwapChains != 0);

		// Get vulkan wrapper object of the queue.
		FVulkanQueue* queue = static_cast<FVulkanQueue*>(pQueue);

		// Create array of vulkan semaphore pointers to wait on.
		std::vector<VkSemaphore> waitSemaphores(numWaitSemaphores);

		// Create array of vulkan swapchain pointers.
		std::vector<VkSwapchainKHR> swapchains(numSwapChains);
		std::vector<uint32_t> swapchainImageIndices(numSwapChains);
		std::vector<VkResult> results(numSwapChains);

		for (uint32_t i = 0; i < numWaitSemaphores; ++i)
		{
			FVulkanSemaphore* semaphore = static_cast<FVulkanSemaphore*>(ppWaitSemaphores[i]);
			RASSERT(semaphore != nullptr);

			waitSemaphores[i] = semaphore->mSemaphore;
		}

		for (uint32_t i = 0; i < numSwapChains; ++i)
		{
			FVulkanSwapChain* swapChain = static_cast<FVulkanSwapChain*>(ppSwapChains[i]);
			RASSERT(swapChain != nullptr);

			swapchains[i] = swapChain->mSwapChain;
			swapchainImageIndices[i] = swapChain->mImageIndex;
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
		if (vkQueuePresentKHR(queue->mQueue, &presentInfo) != VK_SUCCESS)
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

		mCurrentFrameIndex = (mCurrentFrameIndex + 1) % MAX_FRAMES_AT_ONCE;

		// We sucessfully presented our swapchain image.
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::swapChainAcquireNextImage(ISwapChain* pSwapChain, ISemaphore* pSignalSemaphores, IFence* pSignalFence)
	{
		RASSERT(pSwapChain != nullptr);

		FVulkanSwapChain* swapChain = static_cast<FVulkanSwapChain*>(pSwapChain);
		FVulkanSemaphore* semaphore = static_cast<FVulkanSemaphore*>(pSignalSemaphores);
		FVulkanFence* fence = static_cast<FVulkanFence*>(pSignalFence);

		RASSERT(swapChain != nullptr);

		if (fence != nullptr)
		{
			fence->mSubmitted = true;
		}

		if (vkAcquireNextImageKHR(mDevice->mDevice,
			swapChain->mSwapChain,
			UINT64_MAX,
			semaphore != nullptr ? semaphore->mSemaphore : VK_NULL_HANDLE,
			fence != nullptr ? fence->mFence : VK_NULL_HANDLE,
			&swapChain->mImageIndex) != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to acquire next swapchain image.");

			if (fence != nullptr)
			{
				fence->mSubmitted = false;
			}

			return RR_OBJECT_DOES_NOT_EXIST;
		}
		
		if (fence != nullptr)
		{
			if (vkWaitForFences(mDevice->mDevice, 1, &fence->mFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
			{
				RLOG_WARNING("Failed to wait for a fence while aquiring a swapchain image.");
			}

			vkResetFences(mDevice->mDevice, 1, &fence->mFence);
			fence->mSubmitted = false;
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::waitForFence(IFence* pFence)
	{
		RASSERT(pFence != nullptr);
		FVulkanFence* fence = static_cast<FVulkanFence*>(pFence);

		if (!fence->mSubmitted)
		{
			return RR_SUCCESS;
		}

		VkResult res;
		res = vkWaitForFences(mDevice->mDevice, 1, &fence->mFence, VK_TRUE, UINT64_MAX);
		if (res != VK_SUCCESS)
		{
			RLOG_WARNING("Failed to wait for a fence.");
			return RR_FAIL;
		}

		vkResetFences(mDevice->mDevice, 1, &fence->mFence);

		fence->mSubmitted = false;
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::createFrameBuffer(FVulkanFrameBuffer** ppFrameBuffer, FVulkanFrameBufferCreateDesc* pDesc)
	{
		RASSERT(pDesc != nullptr);
		RASSERT(pDesc->numAttachments != 0);

		FVulkanFrameBuffer* fb = new FVulkanFrameBuffer();

		std::vector<VkImageView> attachments(pDesc->numAttachments);

		for (uint32_t i = 0; i < pDesc->numAttachments; ++i)
		{
			FVulkanRenderTarget* target = static_cast<FVulkanRenderTarget*>(pDesc->ppAttachments[i]);
			RASSERT(target != nullptr);

			attachments[i] = target->mTexture->mImageView;
		}

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.pNext = VK_NULL_HANDLE;
		createInfo.flags = 0;
		createInfo.renderPass = pDesc->renderPass->mPass;
		createInfo.attachmentCount = pDesc->numAttachments;
		createInfo.pAttachments = attachments.data();
		createInfo.width = pDesc->width;
		createInfo.height = pDesc->height;
		createInfo.layers = pDesc->layers;
		
		fb->mDesc = *pDesc;

		// Create the framebuffer object
		if (vkCreateFramebuffer(mDevice->mDevice, &createInfo, nullptr, &fb->mFrameBuffer) != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to create a framebuffer object.");
			delete fb;
			return RR_INITIALIZATION_FAILED;
		}

		*ppFrameBuffer = fb;

		return RR_SUCCESS;
	}

    ERenderResult RVulkanRenderer::cmdBegin(ICommandList *pCommandList)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		// Reset this command buffer.
		vkResetCommandBuffer(list->mCmdBuffer, 0);
        list->mCurrentRenderPass = nullptr;
        list->mBoundPipeline = nullptr;

		if (list->mDescriptorSetCache != nullptr)
		{
			list->mDescriptorSetCache->mSetMap.clear();
			vkResetDescriptorPool(mDevice->mDevice, list->mDescriptorSetCache->mDescriptorPool->mDescriptorPool, 0);
		}

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = VK_NULL_HANDLE;

		// Begin recording commands in this command buffer.
		vkBeginCommandBuffer(list->mCmdBuffer, &beginInfo);

        return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdEnd(ICommandList *pCommandList)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		if(list->mCurrentRenderPass != nullptr)
        {
            cmdEndRenderPass(list);
        }

		// Flush commands before ending.
		if(list->mPendingImageBarriers.size() != 0 || list->mPendingBufferBarriers.size() != 0)
		{
			cmdFlushBarriers(list);
		}

		// Stop recording commands in this command list.
		VkResult result = vkEndCommandBuffer(list->mCmdBuffer);
		if (result != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to record commands in command list.");
		}

        list->mCurrentRenderPass = nullptr;
        list->mBoundPipeline = nullptr;

        return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdBeginRenderPass(ICommandList *pCommandList, uint32_t numColorAttachments,
                                                      IRenderTarget **ppColorAttachments,
                                                      IRenderTarget *pDepthStencilAttachment,
													  FLoadStoreOps* pLoadStoreOps)
	{
		RASSERT(pCommandList != nullptr);
		RASSERT(!(numColorAttachments == 0 && pDepthStencilAttachment == nullptr));

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		if(list->mCurrentRenderPass != nullptr)
		{
			cmdEndRenderPass(list);
		}

		// Hash the attachment formats and sample count to check if we already have an existing render pass instance.
		size_t hash = 0;

		for(uint32_t i = 0; i < numColorAttachments; ++i)
		{
			FVulkanRenderTarget* target = static_cast<FVulkanRenderTarget*>(ppColorAttachments[i]);
			hash = hash_bytes(&target->mDesc.format, sizeof(ETextureFormat), hash);
			hash = hash_bytes(&target->mDesc.sampleCount, sizeof(uint32_t), hash);
			hash = hash_bytes(&target->mDesc.sampleQuality, sizeof(uint32_t), hash);
		}

		if(pDepthStencilAttachment != nullptr)
		{
			FVulkanRenderTarget* target = static_cast<FVulkanRenderTarget*>(pDepthStencilAttachment);
			hash = hash_bytes(&target->mDesc.format, sizeof(ETextureFormat), hash);
			hash = hash_bytes(&target->mDesc.sampleCount, sizeof(uint32_t), hash);
			hash = hash_bytes(&target->mDesc.sampleQuality, sizeof(uint32_t), hash);
		}

		size_t hash2 = hash; // we don't need to take the load/store ops in account for frame buffer creation.
		hash = hash_bytes(pLoadStoreOps, sizeof(FLoadStoreOps), hash);

		FVulkanRenderPass* renderPass = nullptr;

		auto renderPassIt = mRenderPassMap.find(hash);
		if(renderPassIt != mRenderPassMap.end())
		{
			// We already have a pre-created render pass.
			renderPass = renderPassIt->second;
		}
		else
		{
			FVulkanRenderPassCreateDesc rpDesc;
			rpDesc.numColorAttachments = numColorAttachments;
			rpDesc.pLoadStoreOps = pLoadStoreOps;

			if(numColorAttachments != 0)
			{
				FVulkanRenderTarget* colorTarget = static_cast<FVulkanRenderTarget*>(ppColorAttachments[0]);
				rpDesc.sampleCount = colorTarget->mTexture->mDesc.sampleCount;
				rpDesc.sampleQuality = colorTarget->mTexture->mDesc.sampleQuality;

				for(uint32_t i = 0; i < numColorAttachments; ++i)
				{
					FVulkanRenderTarget* colorAttachment = static_cast<FVulkanRenderTarget*>(ppColorAttachments[i]);
					rpDesc.colorAttachmentFormats[i] = colorAttachment->mDesc.format;
				}
			}

			if(pDepthStencilAttachment != nullptr)
			{
				FVulkanRenderTarget* depthStencil = static_cast<FVulkanRenderTarget*>(pDepthStencilAttachment);
				rpDesc.depthStencilAttachmentFormat = depthStencil->mDesc.format;

				if (numColorAttachments == 0)
				{
					rpDesc.sampleCount = depthStencil->mTexture->mDesc.sampleCount;
					rpDesc.sampleQuality = depthStencil->mTexture->mDesc.sampleQuality;
				}
			}
			else
			{
				rpDesc.depthStencilAttachmentFormat = ETextureFormat::TEXTURE_FORMAT_UNKNOWN;
			}

			// create a new render pass.
			if(createRenderPass(&renderPass, &rpDesc) != RR_SUCCESS)
			{
				RLOG_ERROR("Failed to create a render pass for the specified formats.");
				return RR_FAIL;
			}

			mRenderPassMap[hash] = renderPass;
		}

		FVulkanFrameBuffer* frameBuffer = nullptr;

		for(uint32_t i = 0; i < numColorAttachments; ++i)
		{
			FVulkanRenderTarget* target = static_cast<FVulkanRenderTarget*>(ppColorAttachments[i]);
			hash2 = hash_bytes(&target->mTexture->mTextureId, sizeof(size_t), hash2);
		}

		if(pDepthStencilAttachment != nullptr)
		{
			FVulkanRenderTarget* target = static_cast<FVulkanRenderTarget*>(pDepthStencilAttachment);
			hash2 = hash_bytes(&target->mTexture->mTextureId, sizeof(size_t), hash2);
		}

		auto frameBufferIt = mFrameBufferMap.find(hash2);
		if(frameBufferIt != mFrameBufferMap.end())
		{
			// We already have a pre-created render pass.
			frameBuffer = frameBufferIt->second;
		}
		else
		{
			FVulkanFrameBufferCreateDesc fbDesc;
			fbDesc.renderPass = renderPass;
			
			std::vector<FVulkanRenderTarget*> attachments(numColorAttachments + (pDepthStencilAttachment != nullptr ? 1 : 0));

			if(numColorAttachments != 0)
			{
				FVulkanRenderTarget* colorTarget = static_cast<FVulkanRenderTarget*>(ppColorAttachments[0]);
				fbDesc.width = colorTarget->mTexture->mDesc.width;
				fbDesc.height = colorTarget->mTexture->mDesc.height;
				fbDesc.layers = colorTarget->mTexture->mDesc.arrayLayers;

				for(uint32_t i = 0; i < numColorAttachments; ++i)
				{
					FVulkanRenderTarget* colorAttachment = static_cast<FVulkanRenderTarget*>(ppColorAttachments[i]);
					attachments[i] = colorAttachment;
				}
			}

			if(pDepthStencilAttachment != nullptr)
			{
				FVulkanRenderTarget* depthAttachment = static_cast<FVulkanRenderTarget*>(pDepthStencilAttachment);
				attachments[numColorAttachments] = depthAttachment;
			}

			fbDesc.numAttachments = (uint32_t)attachments.size();
			fbDesc.ppAttachments = attachments.data();

			// Create a new frame buffer referencing the resources.
			if(createFrameBuffer(&frameBuffer, &fbDesc) != RR_SUCCESS)
			{
				RLOG_ERROR("Failed to create a frame buffer for the specified targets.");
				return RR_FAIL;
			}

			mFrameBufferMap[hash2] = frameBuffer;

		}

		RASSERT(frameBuffer != nullptr);
		RASSERT(renderPass != nullptr);

		VkRect2D renderArea;
		renderArea.offset.x = 0;
		renderArea.offset.y = 0;
		renderArea.extent.width = frameBuffer->mDesc.width;
		renderArea.extent.height = frameBuffer->mDesc.height;

		VkRenderPassBeginInfo beginInfo;
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.pNext = VK_NULL_HANDLE;
		beginInfo.renderPass = renderPass->mPass;
		beginInfo.framebuffer = frameBuffer->mFrameBuffer;
		beginInfo.renderArea = renderArea;

		std::vector<VkClearValue> clearValues(numColorAttachments + (pDepthStencilAttachment != nullptr ? 1 : 0));
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

		vkCmdBeginRenderPass(list->mCmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

		list->mCurrentRenderPass = renderPass;
		//
		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdEndRenderPass(ICommandList *pCommandList)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		if(list->mCurrentRenderPass != nullptr)
		{
			vkCmdEndRenderPass(list->mCmdBuffer);
			list->mCurrentRenderPass = nullptr;
			return RR_SUCCESS;
		}

		return RR_INITIALIZATION_FAILED;
    }

    ERenderResult RVulkanRenderer::cmdBindVertexBuffer(ICommandList *pCommandList, uint32_t numBuffers, IBuffer **ppBuffers,
                                         uint32_t startSlot) {
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		std::vector<VkBuffer> buffers(numBuffers);
		std::vector<VkDeviceSize > offsets(numBuffers);

		// Fill the buffers and offsets vectors with vulkan data.
		for(uint32_t i = 0; i < numBuffers; ++i)
		{
			FVulkanBuffer* buffer = static_cast<FVulkanBuffer*>(ppBuffers[i]);

			buffers[i] = buffer->mBuffer;
			offsets[i] = 0;
		}

		vkCmdBindVertexBuffers(list->mCmdBuffer, startSlot, numBuffers, buffers.data(), offsets.data());
		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdBindIndexBuffer(ICommandList *pCommandList, IBuffer *pBuffer, EIndexType type)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);
		FVulkanBuffer* buffer = static_cast<FVulkanBuffer*>(pBuffer);

		vkCmdBindIndexBuffer(list->mCmdBuffer, buffer->mBuffer, 0, gIndexTypeTranslator[type]);

		return RR_SUCCESS;
    }

    ERenderResult
    RVulkanRenderer::cmdSetViewport(ICommandList *pCommandList, FRect rect, float minDepth, float maxDepth)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		VkViewport viewport;
		viewport.minDepth = minDepth;
		viewport.maxDepth = maxDepth;
		viewport.x = rect.Position.x;
		viewport.y = rect.Position.y;
		viewport.width = rect.Size.x;
		viewport.height = rect.Size.y;

		vkCmdSetViewport(list->mCmdBuffer, 0, 1, &viewport);

		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdSetScissorRects(ICommandList *pCommandList, FRect rect)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		VkRect2D vkRect = {{(int32_t)rect.Position.x, (int32_t)rect.Position.y}, {(uint32_t)rect.Size.x, (uint32_t)rect.Size.y}};

		vkCmdSetScissor(list->mCmdBuffer, 0, 1, &vkRect);

		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdBindPipeline(ICommandList *pCommandList, IPipeline *pPipeline)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);
		FVulkanPipeline* pipeline = static_cast<FVulkanPipeline*>(pPipeline);

		// This pipeline is already bound so we don't have to re-bind.
		if(list->mBoundPipeline == pipeline)
		{
			return RR_SUCCESS;
		}

		list->mBoundPipeline = pipeline;
		vkCmdBindPipeline(list->mCmdBuffer, gPipelineTypeTranslator[pipeline->mType], pipeline->mPipeline);

		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdResourceBarriers(ICommandList *pCommandList, uint32_t numBufferBarriers,
                                                       FBufferBarrier *pBufferBarriers, uint32_t numTextureBarries,
                                                       FTextureBarrier *pTextureBarriers, bool flush)
	{
		RASSERT(pCommandList != nullptr);
		RASSERT(numBufferBarriers < MAX_RESOURCE_BARRIERS);
		RASSERT(numTextureBarries < MAX_RESOURCE_BARRIERS);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		if(numBufferBarriers == 0 && numTextureBarries == 0)
		{
			RLOG_WARNING("For some reason we call resource barriers without any barriers, so we have no need to call the command.");
			return RR_SUCCESS;
		}

		// If we want to record more barriers than we can hold at a single time, flush the current pending barriers.
		if(numBufferBarriers + list->mPendingBufferBarriers.size() > MAX_RESOURCE_BARRIERS
		   || numTextureBarries + list->mPendingImageBarriers.size() > MAX_RESOURCE_BARRIERS)
		{
			cmdFlushBarriers(pCommandList);
		}

		for(uint32_t i = 0; i < numTextureBarries; ++i)
		{
			FTextureBarrier* barrier = &pTextureBarriers[i];
			FVulkanTexture* texture = static_cast<FVulkanTexture*>(barrier->texture);

			auto pendingBarrier = list->mPendingImageBarriers.find(texture);
			if(pendingBarrier != list->mPendingImageBarriers.end())
			{
				// We already have a barrier which applies to this image.
				std::get<1>(pendingBarrier->second) = barrier->requiredState;
			}
			// If we don't have an existing transition, create a new one instead.
			else
			{
				list->mPendingImageBarriers[texture] = std::make_tuple(texture->mState, barrier->requiredState);
			}
		}

		for(uint32_t i = 0; i < numBufferBarriers; ++i)
		{
			FBufferBarrier* barrier = &pBufferBarriers[i];
			FVulkanBuffer* buffer = static_cast<FVulkanBuffer*>(barrier->buffer);

			bool hasExistingTransition = false;

			auto pendingBarrier = list->mPendingBufferBarriers.find(buffer);
			if(pendingBarrier != list->mPendingBufferBarriers.end())
			{
				// We already have a barrier which applies to this image.
				std::get<1>(pendingBarrier->second) = barrier->requiredState;

			}
			// If we don't have an existing transition, create a new one instead.
			else
			{
				list->mPendingBufferBarriers[buffer] = std::make_tuple(buffer->mState, barrier->requiredState);
			}
		}

		// Call the flush command if we want to immediately record these resource barriers into the command buffer.
		if(flush)
		{
			cmdFlushBarriers(pCommandList);
		}

		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdFlushBarriers(ICommandList *pCommandList)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		std::vector<VkBufferMemoryBarrier> bufferBarriers(list->mPendingBufferBarriers.size());
		std::vector<VkImageMemoryBarrier> imageBarriers(list->mPendingImageBarriers.size());

		uint32_t index = 0;

		for(auto it = list->mPendingBufferBarriers.begin(); it != list->mPendingBufferBarriers.end(); ++it)
		{
			VkBufferMemoryBarrier* vkBarrier = &bufferBarriers[index];
			vkBarrier->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			vkBarrier->pNext = VK_NULL_HANDLE;
			vkBarrier->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vkBarrier->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vkBarrier->srcAccessMask = translateResourceStateToAccessMask(it->first->mState);
			vkBarrier->dstAccessMask = translateResourceStateToAccessMask(std::get<1>(it->second));
			vkBarrier->offset = 0;
			vkBarrier->size = VK_WHOLE_SIZE;
			vkBarrier->buffer = it->first->mBuffer;

			it->first->mState = std::get<1>(it->second);
			++index;
		}

		index = 0;
		for(auto it = list->mPendingImageBarriers.begin(); it != list->mPendingImageBarriers.end(); ++it)
		{
			VkImageMemoryBarrier* vkBarrier = &imageBarriers[index];
			vkBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			vkBarrier->image = it->first->mImage;
			vkBarrier->pNext = VK_NULL_HANDLE;
			vkBarrier->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vkBarrier->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vkBarrier->srcAccessMask = translateResourceStateToAccessMask(it->first->mState);
			vkBarrier->dstAccessMask = translateResourceStateToAccessMask(std::get<1>(it->second));
			vkBarrier->oldLayout = translateResourceStateToImageLayout(it->first->mState);
			vkBarrier->newLayout = translateResourceStateToImageLayout(std::get<1>(it->second));
			vkBarrier->subresourceRange.aspectMask = it->first->mVkImageViewDesc.subresourceRange.aspectMask;
			vkBarrier->subresourceRange.baseMipLevel = 0;
			vkBarrier->subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
			vkBarrier->subresourceRange.baseArrayLayer = 0;
			vkBarrier->subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

			it->first->mState = std::get<1>(it->second);

			++index;
		}

		list->mPendingBufferBarriers.clear();
		list->mPendingImageBarriers.clear();

		// Flush pipeline barrier.
		vkCmdPipelineBarrier(list->mCmdBuffer,
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

    ERenderResult RVulkanRenderer::cmdDraw(ICommandList *pCommandList, uint32_t vertexCount, uint32_t vertexOffset)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		RASSERT(list->mBoundPipeline != nullptr);

		if(list->mBoundPipeline != nullptr && list->mBoundPipeline->mType != EPipelineType::PIPELINE_TYPE_GRAPHICS)
		{
			RLOG_WARNING("Tried to call a draw command on a command list where the currently bound pipeline is not a graphics pipeline.");
			return RR_FAIL;
		}

		vkCmdDraw(list->mCmdBuffer, vertexCount, 1, vertexOffset, 0);

		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdDrawIndexed(ICommandList *pCommandList, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		RASSERT(list->mBoundPipeline != nullptr);

		if(list->mBoundPipeline != nullptr && list->mBoundPipeline->mType != EPipelineType::PIPELINE_TYPE_GRAPHICS)
		{
			RLOG_WARNING("Tried to call a draw command on a command list where the currently bound pipeline is not a graphics pipeline.");
			return RR_FAIL;
		}

		vkCmdDrawIndexed(list->mCmdBuffer, indexCount, 1, indexOffset, vertexOffset, 0);

		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdDrawInstanced(ICommandList *pCommandList, uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		RASSERT(list->mBoundPipeline != nullptr);

		if(list->mBoundPipeline != nullptr && list->mBoundPipeline->mType != EPipelineType::PIPELINE_TYPE_GRAPHICS)
		{
			RLOG_WARNING("Tried to call a draw command on a command list where the currently bound pipeline is not a graphics pipeline.");
			return RR_FAIL;
		}

		vkCmdDraw(list->mCmdBuffer, vertexCount, instanceCount, 0, 0);

		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdDrawIndexedInstanced(ICommandList *pCommandList, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset, uint32_t instanceCount)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		RASSERT(list->mBoundPipeline != nullptr);

		if(list->mBoundPipeline != nullptr && list->mBoundPipeline->mType != EPipelineType::PIPELINE_TYPE_GRAPHICS)
		{
			RLOG_WARNING("Tried to call a draw command on a command list where the currently bound pipeline is not a graphics pipeline.");
			return RR_FAIL;
		}

		vkCmdDrawIndexed(list->mCmdBuffer, indexCount, instanceCount, indexOffset, vertexOffset, 0);

		return RR_SUCCESS;
    }

    ERenderResult RVulkanRenderer::cmdDispatch(ICommandList *pCommandList, uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ)
	{
		RASSERT(pCommandList != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		RASSERT(list->mBoundPipeline != nullptr);

		if(list->mBoundPipeline != nullptr && list->mBoundPipeline->mType != EPipelineType::PIPELINE_TYPE_COMPUTE)
		{
			RLOG_WARNING("Tried to call a dispatch command on a command list where the currently bound pipeline is not a compute pipeline.");
			return RR_FAIL;
		}

		// Call dispatch command.
		vkCmdDispatch(list->mCmdBuffer, numGroupsX, numGroupsY, numGroupsZ);

        return RR_SUCCESS;
    }

	ERenderResult RVulkanRenderer::cmdBindDescriptors(ICommandList* pCommandList, uint32_t numDescriptors, FDescriptorBinding* pDescriptors)
	{
		RASSERT(pCommandList != nullptr);
		RASSERT(numDescriptors != 0);

		
		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		// We need to have a pipeline and render pass bound to the pipeline.
		if (list->mBoundPipeline == nullptr || list->mCurrentRenderPass == nullptr)
		{
			return RR_FAIL;
		}

		FVulkanPipeline* pipeline = list->mBoundPipeline;
		FVulkanRenderPass* renderpass = list->mCurrentRenderPass;

		size_t rsHash = 0;

		rsHash = hash_bytes(&pipeline->mPipeline, sizeof(VkPipeline), rsHash);

		for(uint32_t i = 0; i < pipeline->mRootSignature->mDescriptorSetLayouts.size(); ++i)
		{
			rsHash = hash_bytes(&pipeline->mRootSignature->mDescriptorSetLayouts[i], sizeof(VkDescriptorSetLayout), rsHash);
		}

		std::vector<VkDescriptorSet>* descriptorSetTemplates;
		std::vector<VkDescriptorSet> dynamicDescriptorSets;

		// Try to find the descriptor set for this pipeline/descriptor set layout combination.
		auto setCacheIt = mDescriptorSetCache[mCurrentFrameIndex].find(rsHash);

		if(setCacheIt == mDescriptorSetCache[mCurrentFrameIndex].end())
		{
			mDescriptorSetCache[mCurrentFrameIndex][rsHash] = std::vector<VkDescriptorSet>(pipeline->mRootSignature->mDescriptorSetLayouts.size());
	
			VkDescriptorSetAllocateInfo dsAllocInfo;
			dsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			dsAllocInfo.pNext = VK_NULL_HANDLE;
			dsAllocInfo.descriptorPool = mDescriptorPool->mDescriptorPool;
			dsAllocInfo.descriptorSetCount = static_cast<uint32_t>(pipeline->mRootSignature->mDescriptorSetLayouts.size());
			dsAllocInfo.pSetLayouts = pipeline->mRootSignature->mDescriptorSetLayouts.data();

			vkAllocateDescriptorSets(mDevice->mDevice, &dsAllocInfo, mDescriptorSetCache[mCurrentFrameIndex][rsHash].data());
			descriptorSetTemplates = &mDescriptorSetCache[mCurrentFrameIndex][rsHash];
		}
		else
		{
			descriptorSetTemplates = &setCacheIt->second;
		}

		// Create list of descriptor sets we have to update.
		std::vector<VkWriteDescriptorSet> writeSets;
		std::vector<VkCopyDescriptorSet> copySets;
		std::vector<uint32_t> copySetIds;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
		std::vector<VkDescriptorImageInfo> descriptorImageInfos;

		for (uint32_t i = 0; i < numDescriptors; ++i)
		{
			FDescriptorBinding& binding = pDescriptors[i];

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

			switch(binding.type)
			{
				case EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				case EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
				case EDescriptorType::DESCRIPTOR_TYPE_STORAGE_BUFFER:
				case EDescriptorType::DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
				{
					uint32_t firstDescriptor = static_cast<uint32_t>(descriptorBufferInfos.size());

					for(uint32_t j = 0; j < binding.numDescriptors; ++j)
					{
						descriptorBufferInfos.push_back({});
						VkDescriptorBufferInfo& bufferInfo = descriptorBufferInfos.back();
						bufferInfo.buffer = static_cast<FVulkanBuffer*>(binding.ppBuffers[j])->mBuffer;
						bufferInfo.offset = 0;
						bufferInfo.range = VK_WHOLE_SIZE;
					}

					writeSet.pBufferInfo = &descriptorBufferInfos[firstDescriptor];
				}
				break;
				case EDescriptorType::DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
				case EDescriptorType::DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
				{
					RLOG_ERROR("Texel buffers are unimplemented");
				}
					break;
				case EDescriptorType::DESCRIPTOR_TYPE_SAMPLER:
				{
					RLOG_ERROR("Tried to bind a sampler, but a sampler should be compiled with the descriptor set layout.");
					return RR_FAIL;
				}
				break;
				case EDescriptorType::DESCRIPTOR_TYPE_SAMPLED_IMAGE:
				case EDescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE:
				{
                    uint32_t firstDescriptor = static_cast<uint32_t>(descriptorImageInfos.size());

                    for(uint32_t j = 0; j < binding.numDescriptors; ++j)
                    {
                        descriptorImageInfos.push_back({});
                        VkDescriptorImageInfo &imageInfo = descriptorImageInfos.back();
                        imageInfo.imageView = static_cast<FVulkanTexture *>(binding.ppTextures[j])->mImageView;
                        imageInfo.imageLayout = translateResourceStateToImageLayout(static_cast<FVulkanTexture *>(binding.ppTextures[j])->mState);
                        imageInfo.sampler = nullptr;
                    }

					writeSet.pImageInfo = &descriptorImageInfos[firstDescriptor];
				}
				break;

			}

			writeSets.push_back(writeSet);
		}

		if (list->mDescriptorSetCache == nullptr)
		{
			list->mDescriptorSetCache = new FVulkanDynamicDescriptorSetCache();

			FVulkanDescriptorPoolCreateDesc desc;
			desc.flags = 0;
			desc.maxDescriptorSets = gDynamicDescriptorSets;
			desc.numPoolSizes = VK_DESCRIPTOR_TYPE_RANGE_SIZE;
			desc.pPoolSizes = gDynamicDescriptorPoolSizes;

			createDescriptorPool(&list->mDescriptorSetCache->mDescriptorPool, &desc);		
		}

		dynamicDescriptorSets.resize(copySetIds.size());

		// Copy global descriptors into dynamic descriptors
		for(uint32_t i = 0; i < copySetIds.size(); ++i)
		{
			VkDescriptorSetAllocateInfo dsAllocInfo;
			dsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			dsAllocInfo.pNext = VK_NULL_HANDLE;
			dsAllocInfo.descriptorPool = list->mDescriptorSetCache->mDescriptorPool->mDescriptorPool;
			dsAllocInfo.descriptorSetCount = 1;
			dsAllocInfo.pSetLayouts = &pipeline->mRootSignature->mDescriptorSetLayouts[copySetIds[i]];

			vkAllocateDescriptorSets(mDevice->mDevice, &dsAllocInfo, &dynamicDescriptorSets[i]);
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
	
		vkUpdateDescriptorSets(mDevice->mDevice, static_cast<uint32_t>(writeSets.size()), writeSets.data(), static_cast<uint32_t>(copySets.size()), copySets.data());

		for (uint32_t i = 0; i < copySetIds.size(); ++i)
		{
			// Bind descriptor sets.
			vkCmdBindDescriptorSets(list->mCmdBuffer, gPipelineTypeTranslator[pipeline->mType], pipeline->mLayout, copySetIds[i], 1, &dynamicDescriptorSets[i], 0, nullptr);
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::cmdPushConstant(ICommandList* pCommandList, FShaderStageFlags stages, uint32_t size, uint32_t offset, void* values)
	{
		RASSERT(pCommandList != nullptr);
		RASSERT(size % 4 == 0);
		RASSERT(stages != 0);
		RASSERT(offset % 4 == 0);
		RASSERT(values != nullptr);

		// Cast the command list interface to the vulkan type.
		FVulkanCommandList* list = static_cast<FVulkanCommandList*>(pCommandList);

		// Push constants.
		vkCmdPushConstants(list->mCmdBuffer, list->mBoundPipeline->mLayout, shaderStageFlagsToVkShaderStageFlags(stages), offset, size, values);

		return RR_SUCCESS;
	}
}
