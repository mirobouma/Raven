#pragma once
#include "util/enum_as_flags.h"
#include "core/api/rendering/defines.h"
#include "core/api/rendering/image.h"
#include "util/enum_as_flags.h"

typedef enum RenderTargetUsage
{
    RENDER_TARGET_USAGE_COLOR_ATTACHMENT = 1,
    RENDER_TARGET_USAGE_DEPTH_STENCIL_ATTACHMENT = 2,
    MAX_RENDER_TARGET_USAGE = 4
} RenderTargetUsage;
DECL_ENUM_AS_FLAGS(RenderTargetUsage)

struct RenderTargetCreateDesc
{
    RenderTargetUsage 		usage;
    uint32_t 				width;
    uint32_t 				height;
    TextureFormat			format;
    uint32_t				sampleCount;
    uint32_t				sampleQuality;
    ImageTiling			    tiling;
    MemoryType				memoryType;
    void*                   nativeSwapchainImage; // optional
};

struct IRenderTarget
{
    virtual IImage* get_image() const = 0;
    virtual const RenderTargetCreateDesc* get_desc() const = 0;
};
