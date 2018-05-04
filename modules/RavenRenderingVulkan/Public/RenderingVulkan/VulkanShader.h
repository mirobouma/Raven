#pragma once

#include "Rendering/Interfaces/IShader.h"
#include "RenderingVulkan/Vulkan.h"
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