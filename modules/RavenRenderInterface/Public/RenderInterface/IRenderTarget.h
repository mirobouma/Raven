#pragma once
#include "Core/Utility/Enum.h"
#include "RenderInterface/SharedDefines.h"
#include "RenderInterface/IImage.h"
#include "Core/Math/Math.h"

namespace Raven
{
	/*! \enum ERenderTargetUsage
	 \brief Bit flag describing how a render target can be used.
	 */
	typedef enum FRenderTargetUsage
	{
		RENDER_TARGET_USAGE_COLOR_ATTACHMENT = 1 << 0,
		RENDER_TARGET_USAGE_DEPTH_STENCIL_ATTACHMENT = 1 << 1,
		MAX_RENDER_TARGET_USAGE = 1 << 2
	} ERenderTargetUsage;
	DECL_ENUM_AS_FLAGS(FRenderTargetUsage)

	/*! \struct FRenderTargetCreateDesc
	\brief Structure describing the parameters for render target creation. */
	struct FRenderTargetCreateDesc
	{
		FRenderTargetUsage 		usage;
		uint32_t 				width;
		uint32_t 				height;
		ETextureFormat			format;
		uint32_t				sampleCount;
		uint32_t				sampleQuality;
		EImageTiling			tiling;
		EMemoryType				memoryType;
		void*					nativeSwapchainImage; //optional.
	};

	/*! \struct IRenderTarget
	\brief Interface for a render target. */
	struct IRenderTarget 
	{
		virtual IImage* GetImage() const = 0;
		virtual const FRenderTargetCreateDesc* GetDesc() const = 0;
	};
}