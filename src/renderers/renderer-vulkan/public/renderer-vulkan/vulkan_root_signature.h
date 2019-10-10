#pragma once
#include "core/api/rendering/root_signature.h"
#include "renderer-vulkan/vulkan.h"
#include "renderer-vulkan/vulkan_sampler.h"
#include "core/vector.h"
#include <map>


struct VulkanShader;

/*! \struct FVulkanRootSignature
\brief Wrapper object around a vulkan root signature.
*/
struct VulkanRootSignature : public IRootSignature
{
    VulkanShader* pShader; /*!< reference to the shader program this root signature is based on. */
    Vector<VkDescriptorSetLayout>										    descriptorSetLayouts; /*!< Descriptor set layout for this root signature. */
    Vector<VkVertexInputBindingDescription>							        inputBindingDescriptions; /*!< Array of vertex input binding descriptions. */
    Vector<VkVertexInputAttributeDescription>							    inputAttributeDescriptions; /*!< Array of input attribute descriptions. */
};
