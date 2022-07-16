#pragma once
#include "rendering/root_signature.h"
#include "renderer-vulkan/vulkan.h"
#include "renderer-vulkan/vulkan_sampler.h"
#include <vector>
#include <map>

namespace Raven
{
    struct VulkanShader;

    /*! \struct FVulkanRootSignature
    \brief Wrapper object around a vulkan root signature.
    */
    struct VulkanRootSignature : public IRootSignature
    {
        VulkanShader* pShader; /*!< reference to the shader program this root signature is based on. */
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts; /*!< Descriptor set layout for this root signature. */
        std::vector<VkVertexInputBindingDescription> inputBindingDescriptions; /*!< Array of vertex input binding descriptions. */
        std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions; /*!< Array of input attribute descriptions. */
    };
}
