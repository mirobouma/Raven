#pragma once
#include "core/api/rendering/defines.h"

namespace Raven
{
    typedef struct CommandPoolCreateDesc
    {
        // DirectX
        //D3D12_COMMAND_LIST_TYPE	  type;

        // Vulkan
        //VkCommandPoolCreateFlags    flags;
        //uint32_t                    queueFamilyIndex;
        CommandType type;
    } CommandPoolCreateDesc;

    struct ICommandPool {};
}
