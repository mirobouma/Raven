#pragma once
#include "Core/Utility/Enum.h"
#include "SharedDefines.h"

namespace Raven
{
	/*! Flags for queue creation. */
	typedef enum FQueueFlags 
	{
		QUEUE_FLAG_CREATE_PROTECTED_BIT = 1 << 0, // Vulkan-specific flag.
		QUEUE_FLAG_DISABLE_GPU_TIMEOUT = 1 << 1, // DirectX-specific flag.
		MAX_QUEUE_FLAG = 1 << 2
	} FQueueFlags;
	DECL_ENUM_AS_FLAGS(FQueueFlags)
	
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
		FQueueFlags flags; /*! Optional queue creation flags. */
		
	} FQueueCreateDesc;

	/*! \struct IQueue
	\brief Interface for a command queue. */
	struct IQueue {};
}