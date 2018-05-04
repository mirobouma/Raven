#pragma once
#include "Rendering/Interfaces/IBuffer.h"
#include "Core/Debug/LogManager.h"
#include "Vulkan.h"

namespace Raven
{
	/*! \struct FVulkanBuffer
	\brief Wrapper object around a vulkan buffer object.
	*/
	struct FVulkanBuffer : IBuffer
	{
		VkBuffer			mBuffer;
		VkDeviceMemory		mMemory;
		FBufferCreateDesc	mDesc;
	};
}