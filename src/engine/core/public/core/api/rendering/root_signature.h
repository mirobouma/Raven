#pragma once
#include "core/api/rendering/sampler.h"
#include "core/api/rendering/defines.h"

/*! Enumeration describing vertex input rate. */
typedef enum VertexInputRate
{
    VERTEX_INPUT_RATE_VERTEX,
    VERTEX_INPUT_RATE_INSTANCE,
    MAX_VERTEX_INPUT_RATE
} VertexInputRate;

/*! \struct FInputAttributeDescription
\brief Structure describing an attribute binding in the shader program. */
struct InputAttributeDescription
{
    uint32_t		binding;
    uint32_t		location;
    TextureFormat	format;
    uint32_t		offset;
};

/*! \struct FInputBindingDescription
\brief Structure describing an attribute binding in the shader program. */
struct InputBindingDescription
{
    uint32_t            binding;
    uint32_t            stride;
    VertexInputRate     inputRate;
};

/*! \struct FAttributeDescription
\brief Structure describing an attribute binding in the shader program. */
struct InputAssemblerLayout
{
    InputBindingDescription*    pInputBindingDescriptions;
    uint32_t					numInputBindingDescriptions;
    InputAttributeDescription*  pInputAttributeDescriptions;
    uint32_t					numInputAttributeDescriptions;
};

/*! \struct FDescriptorLayoutBinding
\brief Structure describing a resource binding in the shader program. */
struct DescriptorLayoutBinding
{
    uint32_t                set;
    uint32_t                binding;
    DescriptorType          descriptorType;
    uint32_t                descriptorCount;
    ShaderStageFlags	    stageFlags;
    ISampler**              ppImmutableSamplers;
};


/*! \struct FRootSignatureCreateDesc
\brief Structure describing the parameters for root signature creation. */
struct RootSignatureCreateDesc
{
    IShader*                    pShaderProgram;
    InputAssemblerLayout*       pInputAssemblerLayout;
    uint32_t				    numDescriptorBindings;
    DescriptorLayoutBinding*    pDescriptorBindings;
};

/*! \struct IRootSignature
\brief Interface for a root signature. */
struct IRootSignature {};
