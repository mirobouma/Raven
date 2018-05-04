#include "RenderingVulkan/VulkanRenderer.h"
#include "Core/Debug/LogManager.h"
#include "HAL/PlatformFileSystem.h"
#include <iostream>
#include <set>

#undef max
#undef min
#include <algorithm>


#ifdef PLATFORM_LINUX
#include "SDL_syswm.h"
#include <X11/Xlib-xcb.h> // for XGetXCBConnection()
#endif

namespace Raven 
{
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
		}

		// Enable extensions
		if (enableInstanceExtensions(createInfo, instanceExtensions) != VK_SUCCESS)
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

		// Retrieve swap chain images.
		vkGetSwapchainImagesKHR(mDevice->mDevice, swapChain->mSwapChain, &imageCount, nullptr);
		swapChain->mSwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(mDevice->mDevice, swapChain->mSwapChain, &imageCount, swapChain->mSwapChainImages.data());

		// Assign the native window to this swapchain.
		swapChain->mPlatformWindow = pNativeWindow;

		// Create image views.
		swapChain->mSwapChainImageViews.resize(swapChain->mSwapChainImages.size());

		for (uint32_t i = 0; i < swapChain->mSwapChainImages.size(); i++)
		{
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = swapChain->mSwapChainImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = swapChain->mSurfaceFormat.format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(mDevice->mDevice, &viewInfo, nullptr, &swapChain->mSwapChainImageViews[i]) != VK_SUCCESS) 
			{
				RLOG_ERROR("Failed to create image views for the swapchain.");
			}
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

		for (uint32_t i = 0; i < swapChain->mSwapChainImages.size(); ++i)
		{
			vkDestroyImageView(mDevice->mDevice, swapChain->mSwapChainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(mDevice->mDevice, swapChain->mSwapChain, nullptr);
		
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
		poolInfo.flags = 0; // Optional

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
		RASSERT(*ppCommandLists != nullptr);
		RASSERT(pCmdPool != nullptr);

		FVulkanCommandPool* pool = static_cast<FVulkanCommandPool*>(pCmdPool);

		std::vector<VkCommandBuffer> CommandBuffers;
		CommandBuffers.resize(numCommandLists);

		// Create command list wrapper objects.
		for (uint32_t i = 0; i < numCommandLists; ++i)
		{
			ppCommandLists[i] = new FVulkanCommandList();
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
		return destroyCommandLists(pCmdPool, pCommandList, 1);
	}

	ERenderResult RVulkanRenderer::destroyCommandLists(ICommandPool* pCmdPool, ICommandList* pCommandLists, uint32_t numLists)
	{
		RASSERT(pCmdPool != nullptr);
		RASSERT(pCommandLists != nullptr);
		RASSERT(numLists > 0);

		FVulkanCommandPool* pool = static_cast<FVulkanCommandPool*>(pCmdPool);
		FVulkanCommandList* lists = static_cast<FVulkanCommandList*>(pCommandLists);

		std::vector<VkCommandBuffer> buffers;
		buffers.resize(numLists);

		for (uint32_t i = 0; i < numLists; ++i)
		{
			RASSERT(&lists[i] != nullptr);

			buffers[i] = lists[i].mCmdBuffer;
			delete &lists[i];
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
			if (pDesc->pBlendState->renderTargetBlendStates[i].blendEnable)
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

		// Create pipeline layout.
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = 0; // Optional
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &rootSignature->mDescriptorSetLayout;

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
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &rootSignature->mDescriptorSetLayout;

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

		std::vector<VkAttachmentReference> colorAttachmentReferences;
		colorAttachmentReferences.resize(pDesc->numColorAttachments);

		std::vector<VkAttachmentDescription> attachments;
		attachments.resize(pDesc->numColorAttachments);

		uint32_t numAttachments = 0;

		for (uint32_t i = 0; i < pDesc->numColorAttachments; ++i)
		{
			if (pDesc->colorAttachmentFormats[i] != TEXTURE_FORMAT_UNKNOWN)
			{
				attachments[numAttachments] = {};
				attachments[numAttachments].format = gTextureFormatTranslator[pDesc->colorAttachmentFormats[i]];
				attachments[numAttachments].samples = textureSamplesToVkSamples(pDesc->sampleCount);
				attachments[numAttachments].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				attachments[numAttachments].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				attachments[numAttachments].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				attachments[numAttachments].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
				attachments[numAttachments].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachments[numAttachments].finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				
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
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			// define depth attachment reference.
			depthAttachmentRef.attachment = pDesc->numColorAttachments;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachments.push_back(depthAttachment);
		}

		// Describe a subpass.
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = pDesc->colorAttachmentFormats[0] != TEXTURE_FORMAT_UNKNOWN ? numAttachments : 0; // we only have one color attachment.
		subpass.pColorAttachments = pDesc->colorAttachmentFormats[0] != TEXTURE_FORMAT_UNKNOWN ? colorAttachmentReferences.data() : nullptr;
		subpass.pDepthStencilAttachment = pDesc->depthStencilAttachmentFormat != TEXTURE_FORMAT_UNKNOWN ? &depthAttachmentRef : nullptr;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = numAttachments;
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
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
		
		// Create vector of descriptor set bindings.
		std::vector<VkDescriptorSetLayoutBinding> descriptorSetBindings;
		descriptorSetBindings.resize(pDesc->pNumDescriptorBindings);

		// Create descriptor set binding layout.
		for (uint32_t i = 0; i < pDesc->pNumDescriptorBindings; ++i)
		{
			descriptorSetBindings[i] = {};
			descriptorSetBindings[i].binding = pDesc->pDescriptorBindings[i].binding;
			descriptorSetBindings[i].descriptorType = gDescriptorTypeTranslator[pDesc->pDescriptorBindings[i].descriptorType];
			descriptorSetBindings[i].descriptorCount = pDesc->pDescriptorBindings[i].descriptorCount;
			descriptorSetBindings[i].stageFlags = shaderStageFlagsToVkShaderStageFlags(pDesc->pDescriptorBindings[i].stageFlags);

			// Create array of VkSampler objects to bind.
			if (pDesc->pDescriptorBindings[i].descriptorType == DESCRIPTOR_TYPE_SAMPLER)
			{
				RLOG_WARNING("Trying to create static samplers but this has not been fully implemented yet.");
				descriptorSetBindings[i].pImmutableSamplers = nullptr;
			}
			else
			{
				descriptorSetBindings[i].pImmutableSamplers = nullptr;
			}
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetBindings.size());
		layoutInfo.pBindings = descriptorSetBindings.data();

		// Create vulkan descriptor set layout.
		if (vkCreateDescriptorSetLayout(mDevice->mDevice, &layoutInfo, nullptr, &rootSignature->mDescriptorSetLayout) != VK_SUCCESS) 
		{
			RLOG_ERROR("failed to create descriptor set layout!");
			delete rootSignature;
			return RR_INITIALIZATION_FAILED;
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
		vkDestroyDescriptorSetLayout(mDevice->mDevice, rootSignature->mDescriptorSetLayout, nullptr);
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

		// Create image object.
		if (vkCreateImage(mDevice->mDevice, &imageInfo, nullptr, &texture->mImage) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create image!");
			delete texture;
			return RR_INITIALIZATION_FAILED;
		}

		// Set the image description of the texture to the one of the created image.
		texture->mDesc = *pDesc;

		// Get memory requirements for this image.
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(mDevice->mDevice, texture->mImage, &memRequirements);
		uint32_t memoryTypeIndex = findSuitableMemoryType(memRequirements.memoryTypeBits, translateMemoryPropertyFlagsToVk(pDesc->memoryProperties));

		RASSERT(memoryTypeIndex != UINT32_MAX);

		// Allocate memory for this image.
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;

		if (vkAllocateMemory(mDevice->mDevice, &allocInfo, nullptr, &texture->mMemory) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to allocate image memory!");
			delete texture;
			return RR_OUT_OF_DEVICE_MEMORY;
		}

		// Bind allocated memory to the image object.
		vkBindImageMemory(mDevice->mDevice, texture->mImage, texture->mMemory, 0);

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
		texture->mImageView = createImageView(texture, viewType, imageInfo.format, imageAspectFlagFromFormat(imageInfo.format));

		*ppTexture = texture;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyTexture(ITexture* pTexture)
	{
		RASSERT(pTexture != nullptr);

		FVulkanTexture* texture = static_cast<FVulkanTexture*>(pTexture);

		vkDestroyImageView(mDevice->mDevice, texture->mImageView, nullptr);
		vkDestroyImage(mDevice->mDevice, texture->mImage, nullptr);
		vkFreeMemory(mDevice->mDevice, texture->mMemory, nullptr);

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

		// Create image object.
		if (vkCreateBuffer(mDevice->mDevice, &bufferInfo, nullptr, &buffer->mBuffer) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to create buffer!");
			delete buffer;
			return RR_INITIALIZATION_FAILED;
		}

		// Set the image description of the texture to the one of the created image.
		buffer->mDesc = *pDesc;

		// Get memory requirements for this image.
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(mDevice->mDevice, buffer->mBuffer, &memRequirements);
		uint32_t memoryTypeIndex = findSuitableMemoryType(memRequirements.memoryTypeBits, translateMemoryPropertyFlagsToVk(pDesc->memoryProperties));

		RASSERT(memoryTypeIndex != UINT32_MAX);

		// Allocate memory for this image.
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;

		if (vkAllocateMemory(mDevice->mDevice, &allocInfo, nullptr, &buffer->mMemory) != VK_SUCCESS)
		{
			RLOG_ERROR("failed to allocate buffer memory!");
			delete buffer;
			return RR_OUT_OF_DEVICE_MEMORY;
		}

		// Bind allocated memory to the buffer object.
		vkBindBufferMemory(mDevice->mDevice, buffer->mBuffer, buffer->mMemory, 0);


		*ppBuffer = (IBuffer*)buffer;

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::destroyBuffer(IBuffer* pBuffer)
	{
		RASSERT(pBuffer != nullptr);

		FVulkanBuffer* buffer = static_cast<FVulkanBuffer*>(pBuffer);

		vkDestroyBuffer(mDevice->mDevice, buffer->mBuffer, nullptr);
		vkFreeMemory(mDevice->mDevice, buffer->mMemory, nullptr);

		delete pBuffer;

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
		createDesc.memoryProperties = pDesc->memoryProperties;
		createDesc.mipLevels = 1;
		createDesc.sampleCount = pDesc->sampleCount;
		createDesc.sampleQuality = pDesc->sampleQuality;
		createDesc.tiling = pDesc->tiling;
		createDesc.type = ETextureType::TEXTURE_TYPE_2D;
		createDesc.usage = usage;

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

		float queuePriority = 1.0f;

		for (uint32_t i = 0; i < pPhysicalDevice->mQueueFamilyProperties.size(); ++i)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = i;
			queueCreateInfo.queueCount = pPhysicalDevice->mQueueFamilyProperties[i].queueCount;
			queueCreateInfo.pQueuePriorities = &queuePriority;
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

	VkImageView RVulkanRenderer::createImageView(FVulkanTexture* pImage, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = pImage->mImage;
		viewInfo.viewType = viewType;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = pImage->mDesc.baseMipLevel;
		viewInfo.subresourceRange.levelCount = pImage->mDesc.mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = pImage->mDesc.baseArrayLayer;
		viewInfo.subresourceRange.layerCount = pImage->mDesc.arrayLayers * (pImage->mDesc.type == TEXTURE_TYPE_CUBE ? 6 : 1);
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;

		VkImageView imageView;
		if (vkCreateImageView(mDevice->mDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) 
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
		RASSERT(mInitialized);

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
		
		// Submit the commands to the queue.
		if (vkQueueSubmit(queue->mQueue, 1, &submitInfo, fence != nullptr ? fence->mFence : nullptr) != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to submit command lists to the specified command queue.");
			return RR_UNKNOWN_FAILURE;
		}

		// We successfully submitted our commands to the queue.
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
		presentInfo.waitSemaphoreCount = numWaitSemaphores;
		presentInfo.pWaitSemaphores = waitSemaphores.data();
		presentInfo.pResults = results.data();

		// Present our SwapChain images.
		vkQueuePresentKHR(queue->mQueue, &presentInfo);

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

		if (vkAcquireNextImageKHR(mDevice->mDevice,
			swapChain->mSwapChain,
			UINT64_MAX,
			semaphore != nullptr ? semaphore->mSemaphore : VK_NULL_HANDLE,
			fence != nullptr ? fence->mFence : VK_NULL_HANDLE,
			&swapChain->mImageIndex) != VK_SUCCESS)
		{
			RLOG_ERROR("Failed to acquire next swapchain image.");
			return RR_OBJECT_DOES_NOT_EXIST;
		}

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
}
