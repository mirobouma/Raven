#pragma once
#include "Core/Utility/Enum.h"
#include "SharedDefines.h"

namespace Raven
{
	/*! Priority for queue work. */
	typedef enum EQueuePriority {
		QUEUE_PRIORITY_LOW,
		QUEUE_PRIORITY_NORMAL,
		QUEUE_PRIORITY_HIGH,
		QUEUE_PRIORITY_REALTIME,
		MAX_QUEUE_PRIORITY
	} EQueuePriority;

	/*! \struct FQueueCreateDesc
	\brief Structure describing the parameters for command queue creation. */
	typedef struct FQueueCreateDesc
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

		ECommandType type; /*!< Type of commands this queue can accept. */
		EQueuePriority priority; /*! Priority for the work submitted to this queue. */
	} FQueueCreateDesc;

	/*! \struct IQueue
	\brief Interface for a command queue. */
	struct IQueue {};
}