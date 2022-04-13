#pragma once
#include "core/api/rendering/swap_chain.h"
#include "core/os/window.h"
#include "renderer-vulkan/vulkan.h"
#include "renderer-vulkan/vulkan_render_target.h"
#include "renderer-vulkan/vulkan_semaphore.h"
#include "core/vector.h"

namespace Raven
{
    /*! \struct FVulkanSwapChain
    \brief Wrapper object around a vulkan swapchain.
    */
    struct VulkanSwapChain : public ISwapChain
    {
        VkSurfaceKHR					surface; /*!< Surface belonging to the window of the created SwapChain. */
        VkSurfaceCapabilitiesKHR 		surfaceCapabilities;  /*!< Capabilities of the of the native window belonging to the SwapChain. */
        Vector<VkSurfaceFormatKHR>      surfaceFormats; /*!< Supported surface formats of the native window belonging to the SwapChain. */
        VkSurfaceFormatKHR				surfaceFormat; /*!< Current surface format for the swapChain. */
        Vector<VkPresentModeKHR> 	    surfacePresentModes;/*!< Supported present modes of the surface. */
        VkPresentModeKHR				surfacePresentMode; /*!< Current present mode for the swapChain */
        VkSwapchainKHR					swapChain; /*!< Vulkan SwapChain object for this structure. */
        uint32_t						numBackbuffers; /*!< Amount of backbuffers this swapchain has. */
        VkExtent2D						swapChainExtent; /*!< The size of the SwapChain surface. */
        Window* window;  /*!< Reference of the platform window associated with this SwapChain. */
        VulkanRenderTarget** renderTargets; /*!< Render targets for this swapchain. */
        uint32_t						imageIndex; /*!< Current image index to render to on this SwapChain. */
        VulkanSemaphore* imageAcquiredSemaphore; /*!< Image acquired semaphore. */

        /*! Implicit conversion to a VkSwapchainKHR */
        operator VkSwapchainKHR() const { return swapChain; }

        virtual uint32_t get_image_count() const override
        {
            return numBackbuffers;
        }

        virtual uint32_t get_width() const override
        {
            return swapChainExtent.width;
        }

        virtual uint32_t get_height() const override
        {
            return swapChainExtent.height;
        }

        virtual uint32_t get_current_image_index() const override
        {
            return imageIndex;
        }

        virtual IRenderTarget* get_render_target(uint32_t index) const override
        {
            RASSERT(index < numBackbuffers);
            return (IRenderTarget*)renderTargets[index];
        }

        virtual ISemaphore* get_image_acquired_semaphore() const override
        {
            return imageAcquiredSemaphore;
        }
    };
}