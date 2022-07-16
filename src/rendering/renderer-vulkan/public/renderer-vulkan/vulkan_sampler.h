#pragma once
#include "rendering/sampler.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    /*! \struct FVulkanShader
    \brief Wrapper object around a vulkan shader program.
    */
    struct VulkanSampler : public ISampler
    {
        VkSampler			sampler; /*!< Vulkan sampler object. */
        SamplerCreateDesc	desc; /*!< Creation parameters for this sampler. */
    };
}