#pragma once
#include "Math/Math.h"

namespace Raven
{
	/*! \enum ESamplerComparisonState
	\brief Enumeration describing an sampler comparison state.
	 */
	typedef enum ESamplerComparisonFunc
	{
		SAMPLER_COMPARISON_NONE,
		SAMPLER_COMPARISON_GREATER,
		SAMPLER_COMPARISON_GREATER_EQUAL,
		SAMPLER_COMPARISON_LESS,
		SAMPLER_COMPARISON_LESS_EQUAL,
		SAMPLER_COMPARISON_EQUAL,
		MAX_SAMPLER_COMPARISON
	} ESamplerComparisonFunc;

	/*! \enum ESamplerFilter
	\brief Enumeration describing an sampler filter type.
	 */
	typedef enum ESamplerFilter
	{
		SAMPLER_FILTER_LINEAR,
		SAMPLER_FILTER_NEAREST,
		MAX_SAMPLER_FILTER

	} ESamplerFilter;

	/*! \enum ESamplerAddressMode
	\brief Enumeration describing sampler adress modes.
	 */
	typedef enum ESamplerAddressMode
	{
		SAMPLER_ADDRESS_MODE_REPEAT,
		SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
		MAX_SAMPLER_ADDRESS_MODE
	} ESamplerAddressMode;

	/*! \enum EMipMode
	\brief Enumeration describing sampler mip modes.
	 */
	typedef enum EMipMode
	{
		MIP_MODE_LINEAR,
		MIP_MODE_NEAREST,
		MAX_MIP_MODE
	} EMipMode;

	/*! \enum EBorderColor
	\brief Enumeration describing the sampler border color.
	*/
	typedef enum EBorderColor
	{
		BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
		BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
		BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
		BORDER_COLOR_INT_OPAQUE_BLACK = 3,
		BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
		BORDER_COLOR_INT_OPAQUE_WHITE = 5,
		MAX_BORDER_COLOR
	} EBorderColor;

	/*! \struct FSamplerCreateDesc
	\brief Structure describing the parameters for sampler creation. */
	struct FSamplerCreateDesc
	{
		ESamplerFilter 			magFilter;
		ESamplerFilter			minFilter;
		ESamplerAddressMode 	addressModeU;
		ESamplerAddressMode 	addressModeV;
		ESamplerAddressMode 	addressModeW;
		bool 					anisotropyEnable;
		float 					maxAnisotropy;
		EBorderColor			borderColor;
		ESamplerComparisonFunc 	comparisonFunc;
		EMipMode 				mipmapMode;
		float 					mipLodBias;
		float 					minLod;
		float 					maxLod;
	};

	/*! \struct ISampler
	\brief Interface for a sampler. */
	struct ISampler {};
}