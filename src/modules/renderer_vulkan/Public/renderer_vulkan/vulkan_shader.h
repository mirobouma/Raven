#pragma once

#include "renderer_interface/shader.h"
#include "renderer_vulkan/vulkan.h"
#include <vector>

namespace Raven
{
	/*! \struct FVulkanShader
	\brief Wrapper object around a vulkan shader program.
	*/
	struct FVulkanShader : public IShader
	{
		VkShaderModule	mShaders[MAX_SHADER_STAGE]; /*!< Shader modules. */
	};
}