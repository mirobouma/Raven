#pragma once
#include "core/api/rendering/pipeline.h"
#include "renderer-vulkan/vulkan.h"

namespace Raven
{
    struct VulkanRootSignature;

    /*! \struct FVulkanPipeline
    \brief Wrapper object around vulkan pipeline.
    */
    struct VulkanPipeline : public IPipeline
    {
        PipelineType			type; /*! Type of this pipeline object. */
        VkPipeline				pipeline; /*!< Vulkan pipeline object. */
        VkPipelineLayout		layout; /*!< Pipeline layout for this pipeline. */
        VulkanRootSignature* rootSignature; /*! Descriptor set layout for this pipeline state. */
    };
}
