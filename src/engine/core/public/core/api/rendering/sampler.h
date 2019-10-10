#pragma once

/*! \enum SamplerComparisonState
\brief Enumeration describing an sampler comparison state.
 */
typedef enum SamplerComparisonFunc
{
    SAMPLER_COMPARISON_NONE,
    SAMPLER_COMPARISON_GREATER,
    SAMPLER_COMPARISON_GREATER_EQUAL,
    SAMPLER_COMPARISON_LESS,
    SAMPLER_COMPARISON_LESS_EQUAL,
    SAMPLER_COMPARISON_EQUAL,
    MAX_SAMPLER_COMPARISON
} SamplerComparisonFunc;

/*! \enum SamplerFilter
\brief Enumeration describing an sampler filter type.
 */
typedef enum SamplerFilter
{
    SAMPLER_FILTER_LINEAR,
    SAMPLER_FILTER_NEAREST,
    MAX_SAMPLER_FILTER

} SamplerFilter;

/*! \enum SamplerAddressMode
\brief Enumeration describing sampler adress modes.
 */
typedef enum SamplerAddressMode
{
    SAMPLER_ADDRESS_MODE_REPEAT,
    SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
    SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
    MAX_SAMPLER_ADDRESS_MODE
} SamplerAddressMode;

/*! \enum MipMode
\brief Enumeration describing sampler mip modes.
 */
typedef enum MipMode
{
    MIP_MODE_LINEAR,
    MIP_MODE_NEAREST,
    MAX_MIP_MODE
} MipMode;

/*! \enum EBorderColor
\brief Enumeration describing the sampler border color.
*/
typedef enum BorderColor
{
    BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
    BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
    BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
    BORDER_COLOR_INT_OPAQUE_BLACK = 3,
    BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
    BORDER_COLOR_INT_OPAQUE_WHITE = 5,
    MAX_BORDER_COLOR
} BorderColor;

/*! \struct FSamplerCreateDesc
\brief Structure describing the parameters for sampler creation. */
struct SamplerCreateDesc
{
    SamplerFilter magFilter;
    SamplerFilter minFilter;
    SamplerAddressMode 	addressModeU;
    SamplerAddressMode 	addressModeV;
    SamplerAddressMode 	addressModeW;
    bool anisotropyEnable;
    float maxAnisotropy;
    BorderColor	borderColor;
    SamplerComparisonFunc comparisonFunc;
    MipMode mipmapMode;
    float mipLodBias;
    float minLod;
    float maxLod;
};

/*! \struct ISampler
\brief Interface for a sampler. */
struct ISampler {};
