#pragma once
#include "core/api/rendering/defines.h"

namespace Raven
{
    typedef enum QueuePriority {
        QUEUE_PRIORITY_LOW,
        QUEUE_PRIORITY_NORMAL,
        QUEUE_PRIORITY_HIGH,
        QUEUE_PRIORITY_REALTIME,
        MAX_QUEUE_PRIORITY
    } QueuePriority;

    typedef struct QueueCreateDesc
    {
        // DX12
        //D3D12_COMMAND_LIST_TYPE   Type;
        //INT                       Priority;
        //D3D12_COMMAND_QUEUE_FLAGS Flags;
        //UINT                      NodeMask;

        // VULKAN
        //VkDeviceQueueCreateFlags    flags;
        //uint32_t                    queueFamilyIndex;
        //uint32_t                    queueCount;
        //const float*                pQueuePriorities;

        CommandType type; /*!< Type of commands this queue can accept. */
        QueuePriority priority; /*! Priority for the work submitted to this queue. */
    } QueueCreateDesc;

    /*! \struct IQueue
    \brief Interface for a command queue. */
    struct IQueue {};
}