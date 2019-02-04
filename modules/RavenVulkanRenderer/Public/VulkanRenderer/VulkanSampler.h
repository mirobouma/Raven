#pragma once

#include "RenderInterface/ISampler.h"
#include "VulkanRenderer/Vulkan.h"
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