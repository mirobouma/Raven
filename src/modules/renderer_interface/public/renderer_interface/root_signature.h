#pragma once
#include "renderer_interface/sampler.h"
#include "renderer_interface/shared_defines.h"

namespace Raven
{
	/*! Enumeration describing vertex input rate. */
	typedef enum EVertexInputRate
	{
		VERTEX_INPUT_RATE_VERTEX,
		VERTEX_INPUT_RATE_INSTANCE,
		MAX_VERTEX_INPUT_RATE
	} EVertexInputRate;

	/*! \struct FInputAttributeDescription
	\brief Structure describing an attribute binding in the shader program. */
	struct FInputAttributeDescription
	{
		uint32_t		binding;
		uint32_t		location;
		ETextureFormat	format;
		uint32_t		offset;
	};
	
	/*! \struct FInputBindingDescription
	\brief Structure describing an attribute binding in the shader program. */
	struct FInputBindingDescription
	{
		uint32_t             binding;
		uint32_t             stride;
		EVertexInputRate     inputRate;
	};

	/*! \struct FAttributeDescription
	\brief Structure describing an attribute binding in the shader program. */
	struct FInputAssemblerLayout
	{
		FInputBindingDescription*	pInputBindingDescriptions;
		uint32_t					numInputBindingDescriptions;
		FInputAttributeDescription* pInputAttributeDescriptions;
		uint32_t					numInputAttributeDescriptions;
	};

	/*! \struct FDescriptorLayoutBinding
	\brief Structure describing a resource binding in the shader program. */
	struct FDescriptorLayoutBinding
	{
		uint32_t              set;
		uint32_t              binding;
		EDescriptorType       descriptorType;
		uint32_t              descriptorCount;
		FShaderStageFlags	  stageFlags;
		ISampler**			  ppImmutableSamplers;
	};

	//struct FDescriptorSet
	//{
	//	uint32_t				numDescriptorBindings;
	//	FDescriptorBinding*		pDescriptorBindings;
	//};

	/*! \struct FRootSignatureCreateDesc
	\brief Structure describing the parameters for root signature creation. */
	struct FRootSignatureCreateDesc
	{
		IShader*				pShaderProgram;
		FInputAssemblerLayout*	pInputAssemblerLayout;
		//uint32_t				numDescriptorSets;
		//FDescriptorSet*			pDescriptorSets;
		uint32_t				numDescriptorBindings;
		FDescriptorLayoutBinding*pDescriptorBindings;
		//uint32_t				numStaticSamplers;
		//ISampler*				pStaticSamplers;
	};

	/*! \struct IRootSignature
	\brief Interface for a root signature. */
	struct IRootSignature {};
}