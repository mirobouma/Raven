#pragma once

#include "RenderInterface/IShader.h"
#include "VulkanRenderer/Vulkan.h"
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