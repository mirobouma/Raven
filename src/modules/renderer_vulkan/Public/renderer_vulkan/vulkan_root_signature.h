#pragma once

#include "renderer_interface/root_signature.h"
#include "renderer_vulkan/vulkan.h"
#include "renderer_vulkan/vulkan_sampler.h"
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