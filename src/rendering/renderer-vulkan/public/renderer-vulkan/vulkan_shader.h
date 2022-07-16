#pragma once
#include "rendering/shader.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    /*! \struct FVulkanShader
    \brief Wrapper object around a vulkan shader program.
    */
    struct VulkanShader : public IShader
    {
        VkShaderModule	shaders[MAX_SHADER_STAGE]; /*!< Shader modules. */
    };
}
