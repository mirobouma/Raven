#pragma once
#include "Rendering/Interfaces/IPipeline.h"
#include "RenderingVulkan/Vulkan.h"

namespace Raven
{
	struct FVulkanRootSignature;

	/*! \struct FVulkanPipeline
	\brief Wrapper object around vulkan pipeline.
	*/
	struct FVulkanPipeline : public IPipeline
	{
		EPipelineType			mType; /*! Type of this pipeline object. */
		VkPipeline				mPipeline; /*!< Vulkan pipeline object. */
		VkPipelineLayout		mLayout; /*!< Pipeline layout for this pipeline. */
		FVulkanRootSignature*	mRootSignature; /*! Descriptor set layout for this pipeline state. */
	};
}