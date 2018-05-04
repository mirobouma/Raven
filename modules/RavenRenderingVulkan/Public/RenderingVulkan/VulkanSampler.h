#pragma once

#include "Rendering/Interfaces/ISampler.h"
#include "RenderingVulkan/Vulkan.h"
#include <vector>

namespace Raven
{
	/*! \struct FVulkanShader
	\brief Wrapper object around a vulkan shader program.
	*/
	struct FVulkanSampler : public ISampler
	{
		VkSampler			mSampler; /*!< Vulkan sampler object. */
		FSamplerCreateDesc	mDesc; /*!< Creation parameters for this sampler. */
	};
}