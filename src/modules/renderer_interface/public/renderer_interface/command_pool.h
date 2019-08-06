#pragma once
#include "renderer_interface/shared_defines.h"

namespace Raven
{
	/*! \struct FCommandPoolCreateDesc
	\brief Structure describing the parameters for command pool creation. */
	typedef struct FCommandPoolCreateDesc
	{
		// DirectX
		//D3D12_COMMAND_LIST_TYPE	  type;

		// Vulkan
		//VkCommandPoolCreateFlags    flags;
		//uint32_t                    queueFamilyIndex;
		ECommandType type;
	} FCommandPoolCreateDesc;

	/*! \struct ICommandPool
	\brief Interface for a command pool. */
	struct ICommandPool {};
}