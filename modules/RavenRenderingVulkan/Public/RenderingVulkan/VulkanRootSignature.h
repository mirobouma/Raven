#pragma once

#include "Rendering/Interfaces/IRootSignature.h"
#include "RenderingVulkan/Vulkan.h"
#include "RenderingVulkan/VulkanSampler.h"
#include <vector>
#include <map>

namespace Raven
{
	struct FVulkanShader;

	/*! \struct FVulkanRootSignature
	\brief Wrapper object around a vulkan root signature.
	*/
	struct FVulkanRootSignature : public IRootSignature
	{
		FVulkanShader*															mpShader; /*!< reference to the shader program this root signature is based on. */
		std::vector<VkDescriptorSetLayout>										mDescriptorSetLayouts; /*!< Descriptor set layout for this root signature. */
		std::vector<VkVertexInputBindingDescription>							mInputBindingDescriptions; /*!< Array of vertex input binding descriptions. */
		std::vector<VkVertexInputAttributeDescription>							mInputAttributeDescriptions; /*!< Array of input attribute descriptions. */
		//std::vector<FVulkanSampler*>											mStaticSamplers; /*!< Pointer to a list of static samplers. */
	};
}