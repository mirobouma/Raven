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
		VkSurfaceKHR					mSurface; /*!< Surface belonging to the window of the created SwapChain. */
		VkSurfaceCapabilitiesKHR 		mSurfaceCapabilities;  /*!< Capabilities of the of the native window belonging to the SwapChain. */
		std::vector<VkSurfaceFormatKHR> mSurfaceFormats; /*!< Supported surface formats of the native window belonging to the SwapChain. */
		std::vector<VkPresentModeKHR> 	mSurfacePresentModes;/*!< Supported present modes of the surface. */
		VkSwapchainKHR					mSwapChain; /*!< Vulkan SwapChain object for this structure. */
		std::vector<VkImage>			mSwapChainImages; /*!< Vector containing a VkImage for each backbuffer in the swapchain. */
		std::vector<VkImageView>		mSwapChainImageViews; /*!< Vector containing a VkImageView for each VkImage in SwapChainImages. */
		VkFormat						mSwapChainImageFormat; /*!< The format for the swapchain images. */
		VkExtent2D						mSwapChainExtent; /*!< The size of the swapchain surface. */
		IPlatformWindow*				mPlatformWindow;  /*!< Reference of the platform window associated with this swapchain. */

		/*! Implicit conversion to a VkSwapchainKHR */
		operator VkSwapchainKHR() const { return mSwapChain; }
	};
}