#pragma once
#include "Rendering/Interfaces/ISwapChain.h"
#include "Platform/Interfaces/IPlatformWindow.h"
#include "RenderingVulkan/Vulkan.h"
#include <vector>

namespace Raven
{
	/*! \struct FQueueIndices
	\brief Structure containing all the queue indices for a physical device.
	*/
	struct FQueueFamilyIndices
	{
		/*! Check if the present queue is unique. */
		bool HasDedicatedComputeQueue() const { return ComputeQueueIndex != GraphicsQueueIndex; }

		/*! Check if the transfer queue is unique. */
		bool HasDedicatedTransferQueue() const { return TransferQueueIndex != GraphicsQueueIndex && TransferQueueIndex != ComputeQueueIndex; }

		uint32_t	GraphicsQueueIndex; /*!< graphics queue index. */
		uint32_t	ComputeQueueIndex; /*!< compute queue index. */
		uint32_t	TransferQueueIndex; /*!< transfer queue index. */
	};

	/*! \struct FPhysicalVulkanDevice
	\brief Wrapper object around a vulkan physical device.
	*/
	struct FVulkanPhysicalDevice
	{
		VkPhysicalDevice						PhysicalDevice; /*!< Reference to the physical device associated with this vulkan logical device.*/
		VkPhysicalDeviceProperties				Properties; /*!< Properties of this physical device. */
		VkPhysicalDeviceFeatures				Features; /*!< Features that this physical device supports. */
		VkPhysicalDeviceMemoryProperties		MemoryProperties; /*!< Memory properties of this physical device. */
		std::vector<VkQueueFamilyProperties>	QueueFamilyProperties; /*!< Queue family properties of this physical device. */
		std::vector<VkExtensionProperties>		SupportedExtensions; /*!< Extensions that this physical device supports. */

        /*! If a vulkan function requires the VkPhysicalDevice and we pass the FVulkanPhysicalDevice object, return PhysicalDevice. */
        explicit operator VkPhysicalDevice() { return PhysicalDevice; }
	};

	/*! \struct FVulkanDevice
	\brief Wrapper object around a vulkan logical device.
	*/
	struct FVulkanDevice
	{
		VkDevice					Device; /*!< Logical device. */
		VkQueue						GraphicsQueue; /*!< graphics queue. */
		VkQueue						ComputeQueue; /*!< compute queue. */
		VkQueue						TransferQueue; /*!< transfer queue. */
		FQueueFamilyIndices			QueueIndices; /*!< Queue indices on the physical device. */
		FVulkanPhysicalDevice*		PhysicalDevice; /*!< Reference to the physical device associated with this vulkan logical device.*/
		VkPhysicalDeviceFeatures	EnabledFeatures;/*!< Features that have been enabled for use on the physical device. */

        /*! If a vulkan function requires the VkDevice and we pass the FVulkanDevice object, return Device. */
        explicit operator VkDevice() { return Device; }
	};

}