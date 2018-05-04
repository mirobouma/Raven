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
		VkSurfaceFormatKHR				mSurfaceFormat; /*!< Current surface format for the swapChain. */
		std::vector<VkPresentModeKHR> 	mSurfacePresentModes;/*!< Supported present modes of the surface. */
		VkPresentModeKHR				mSurfacePresentMode; /*!< Current present mode for the swapChain */
		VkSwapchainKHR					mSwapChain; /*!< Vulkan SwapChain object for this structure. */
		std::vector<VkImage>			mSwapChainImages; /*!< Vector containing a VkImage for each backbuffer in the SwapChain. */
		std::vector<VkImageView>		mSwapChainImageViews; /*!< Vector containing a VkImageView for each VkImage in SwapChainImages. */
		VkExtent2D						mSwapChainExtent; /*!< The size of the SwapChain surface. */
		IPlatformWindow*				mPlatformWindow;  /*!< Reference of the platform window associated with this SwapChain. */
		uint32_t						mImageIndex; /*!< Current image index to render to on this SwapChain. */
		/*! Implicit conversion to a VkSwapchainKHR */
		operator VkSwapchainKHR() const { return mSwapChain; }
	};
}