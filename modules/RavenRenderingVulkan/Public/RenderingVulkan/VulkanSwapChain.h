#pragma once
#include "Rendering/Interfaces/ISwapChain.h"
#include "Platform/Interfaces/IPlatformWindow.h"
#include "RenderingVulkan/Vulkan.h"
#include <vector>

namespace Raven 
{
	/*! \struct FVulkanSwapChain
	\brief Wrapper object around a vulkan swapchain.
	*/
	struct FVulkanSwapChain : public ISwapChain 
	{
		VkSurfaceKHR				Surface; /*!< Surface belonging to the window of the created swapchain. */
		VkSwapchainKHR				SwapChain; /*!< Vulkan swapchain. */
		std::vector<VkImage>		SwapChainImages; /*!< Vector containing a VkImage for each backbuffer in the swapchain. */
		std::vector<VkImageView>	SwapChainImageViews; /*!< Vector containing a VkImageView for each VkImage in SwapChainImages. */
		VkFormat					SwapChainImageFormat; /*!< The format for the swapchain images. */
		VkExtent2D					SwapChainExtent; /*!< The size of the swapchain surface. */
		IPlatformWindow*			PlatformWindow;  /*!< Reference of the platform window associated with this swapchain. */
	};
}