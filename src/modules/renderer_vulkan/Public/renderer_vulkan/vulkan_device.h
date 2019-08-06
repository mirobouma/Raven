#pragma once
#include "renderer_interface/swapchain.h"
#include "platform/interfaces/window.h"
#include "renderer_vulkan/vulkan.h"
#include <vector>
#include <map>

namespace Raven
{
	/*! \struct FQueueIndices
	\brief Structure containing all the queue indices for a physical device.
	*/
	struct FQueueFamilyIndices
	{
		/*! Check if the present queue is unique. */
		bool hasDedicatedComputeQueue() const { return mComputeQueueIndex != mGraphicsQueueIndex; }

		/*! Check if the transfer queue is unique. */
		bool hasDedicatedTransferQueue() const { return mTransferQueueIndex != mGraphicsQueueIndex && mTransferQueueIndex != mComputeQueueIndex; }

		uint32_t	mGraphicsQueueIndex; /*!< graphics queue index. */
		uint32_t	mComputeQueueIndex; /*!< compute queue index. */
		uint32_t	mTransferQueueIndex; /*!< transfer queue index. */
	};

	/*! \struct FPhysicalVulkanDevice
	\brief Wrapper object around a vulkan physical device.
	*/
	struct FVulkanPhysicalDevice
	{
		VkPhysicalDevice						mPhysicalDevice; /*!< Reference to the physical device associated with this vulkan logical device.*/
		VkPhysicalDeviceProperties				mProperties; /*!< Properties of this physical device. */
		VkPhysicalDeviceFeatures				mFeatures; /*!< Features that this physical device supports. */
		VkPhysicalDeviceMemoryProperties		mMemoryProperties; /*!< Memory properties of this physical device. */
		std::vector<VkQueueFamilyProperties>	mQueueFamilyProperties; /*!< Queue family properties of this physical device. */
		std::vector<VkExtensionProperties>		mSupportedExtensions; /*!< Extensions that this physical device supports. */

		/*! Implicit conversion to a VkPhysicalDevice */
        operator VkPhysicalDevice() const { return mPhysicalDevice; }
	};

	/*! \struct FVulkanDevice
	\brief Wrapper object around a vulkan logical device.
	*/
	struct FVulkanDevice
	{
		VkDevice						mDevice; /*!< Logical device. */
		FQueueFamilyIndices				mQueueIndices; /*!< Queue indices on the physical device. */
		std::map<uint32_t, uint32_t>	mQueueCounters; /*!< Counters for retrieved queues. */
		FVulkanPhysicalDevice*			mPhysicalDevice; /*!< Reference to the physical device associated with this vulkan logical device.*/
		VkPhysicalDeviceFeatures		mEnabledFeatures;/*!< Features that have been enabled for use on the physical device. */

		/*! Implicit conversion to a VkDevice */
		operator VkDevice() const { return mDevice; }
	};

}