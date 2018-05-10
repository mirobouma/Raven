#pragma once
#include "Rendering/Interfaces/ISampler.h"
#include "Rendering/Interfaces/SharedDefines.h"

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

	/*! Enumeration describing a descriptor type. */
	typedef enum FShaderStageFlags
	{
		SHADER_STAGE_FLAG_VERTEX = 0x00000001,
		SHADER_STAGE_FLAG_TESSELLATION_CONTROL = 0x00000002,
		SHADER_STAGE_FLAG_TESSELLATION_EVALUATION = 0x00000004,
		SHADER_STAGE_FLAG_GEOMETRY = 0x00000008,
		SHADER_STAGE_FLAG_FRAGMENT = 0x00000010,
		SHADER_STAGE_FLAG_COMPUTE = 0x00000020,
		SHADER_STAGE_FLAG_ALL_GRAPHICS = 0x0000001F,
		SHADER_STAGE_FLAG_ALL = 0x7FFFFFFF,
	} FShaderStageFlags;
	DECL_ENUM_AS_FLAGS(FShaderStageFlags);

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