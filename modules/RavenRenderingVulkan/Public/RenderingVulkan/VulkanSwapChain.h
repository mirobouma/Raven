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
		VkSurfaceKHR					Surface; /*!< Surface belonging to the window of the created SwapChain. */
		VkSurfaceCapabilitiesKHR 		SurfaceCapabilities;  /*!< Capabilities of the of the native window belonging to the SwapChain. */
		std::vector<VkSurfaceFormatKHR> SurfaceFormats; /*!< Supported surface formats of the native window belonging to the SwapChain. */
		std::vector<VkPresentModeKHR> 	SurfacePresentModes;/*!< Supported present modes of the surface. */
		VkSwapchainKHR					SwapChain; /*!< Vulkan SwapChain object for this structure. */
		std::vector<VkImage>			SwapChainImages; /*!< Vector containing a VkImage for each backbuffer in the swapchain. */
		std::vector<VkImageView>		SwapChainImageViews; /*!< Vector containing a VkImageView for each VkImage in SwapChainImages. */
		VkFormat						SwapChainImageFormat; /*!< The format for the swapchain images. */
		VkExtent2D						SwapChainExtent; /*!< The size of the swapchain surface. */
		IPlatformWindow*				PlatformWindow;  /*!< Reference of the platform window associated with this swapchain. */

		/*! If a vulkan function requires the VkSwapchainKHR and we pass the FVulkanSwapChain object, return SwapChain. */
		explicit operator VkSwapchainKHR() { return SwapChain; }
	};
}