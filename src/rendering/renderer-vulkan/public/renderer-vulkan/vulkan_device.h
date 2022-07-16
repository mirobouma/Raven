#pragma once
#include "rendering/swap_chain.h"
#include "os/native_window.h"
#include "renderer-vulkan/vulkan.h"
#include <map>
#include <vector>

namespace Raven
{
    /*! \struct FQueueIndices
    \brief Structure containing all the queue indices for a physical device.
    */
    struct QueueFamilyIndices
    {
        /*! Check if the present queue is unique. */
        bool has_dedicated_compute_queue() const { return computeQueueIndex != graphicsQueueIndex; }

        /*! Check if the transfer queue is unique. */
        bool has_dedicated_transfer_queue() const { return transferQueueIndex != graphicsQueueIndex && transferQueueIndex != computeQueueIndex; }

        uint32_t	graphicsQueueIndex; /*!< graphics queue index. */
        uint32_t	computeQueueIndex; /*!< compute queue index. */
        uint32_t	transferQueueIndex; /*!< transfer queue index. */
    };

    /*! \struct FPhysicalVulkanDevice
    \brief Wrapper object around a vulkan physical device.
    */
    struct VulkanPhysicalDevice
    {
        VkPhysicalDevice						physicalDevice; /*!< Reference to the physical device associated with this vulkan logical device.*/
        VkPhysicalDeviceProperties				properties; /*!< Properties of this physical device. */
        VkPhysicalDeviceFeatures				features; /*!< Features that this physical device supports. */
        VkPhysicalDeviceMemoryProperties		memoryProperties; /*!< Memory properties of this physical device. */
        std::vector<VkQueueFamilyProperties>	        queueFamilyProperties; /*!< Queue family properties of this physical device. */
        std::vector<VkExtensionProperties>		    supportedExtensions; /*!< Extensions that this physical device supports. */

        /*! Implicit conversion to a VkPhysicalDevice */
        operator VkPhysicalDevice() const { return physicalDevice; }
    };

    /*! \struct FVulkanDevice
    \brief Wrapper object around a vulkan logical device.
    */
    struct VulkanDevice
    {
        VkDevice						device; /*!< Logical device. */
        QueueFamilyIndices				queueIndices; /*!< Queue indices on the physical device. */
        std::map<uint32_t, uint32_t>	queueCounters; /*!< Counters for retrieved queues. */
        VulkanPhysicalDevice* physicalDevice; /*!< Reference to the physical device associated with this vulkan logical device.*/
        VkPhysicalDeviceFeatures		enabledFeatures; /*!< Features that have been enabled for use on the physical device. */

        /*! Implicit conversion to a VkDevice */
        operator VkDevice() const { return device; }
    };
}
