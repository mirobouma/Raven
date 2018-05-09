#pragma once
#include "Rendering/Interfaces/ISwapChain.h"
#include "Platform/Interfaces/IPlatformWindow.h"
#include "RenderingVulkan/Vulkan.h"
#include "RenderingVulkan/VulkanRenderTarget.h"
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
		uint32_t						mNumBackbuffers; /*!< Amount of backbuffers this swapchain has. */
		VkExtent2D						mSwapChainExtent; /*!< The size of the SwapChain surface. */
		IPlatformWindow*				mPlatformWindow;  /*!< Reference of the platform window associated with this SwapChain. */
		FVulkanRenderTarget**			mRenderTargets; /*!< Render targets for this swapchain. */
		uint32_t						mImageIndex; /*!< Current image index to render to on this SwapChain. */

		/*! Implicit conversion to a VkSwapchainKHR */
		operator VkSwapchainKHR() const { return mSwapChain; }

		virtual uint32_t getImageCount() const override
		{
			return mNumBackbuffers;
		}

		virtual uint32_t getWidth() const override
		{
			return mSwapChainExtent.width;
		}

		virtual uint32_t getHeight() const override
		{
			return mSwapChainExtent.height;
		}

		virtual uint32_t getCurrentImageIndex() const override
		{
			return mImageIndex;
		}

		virtual IRenderTarget* getRenderTarget(uint32_t index) const override
		{
			RASSERT(index < mNumBackbuffers);
			return (IRenderTarget*)mRenderTargets[index];
		}
	};
}