#pragma once

#include "RenderingVulkan/Vulkan.h"
#include "Rendering/Interfaces/IRenderer.h"
#include "RenderingVulkan/VulkanDevice.h"
#include "RenderingVulkan/VulkanSwapChain.h"
#include "RenderingVulkan/VulkanPipeline.h"
#include "RenderingVulkan/VulkanTexture.h"
#include "RenderingVulkan/VulkanBuffer.h"
#include "RenderingVulkan/VulkanQueue.h"
#include "RenderingVulkan/VulkanShader.h"
#include "RenderingVulkan/VulkanFence.h"
#include "RenderingVulkan/VulkanSemaphore.h"
#include "RenderingVulkan/VulkanDescriptorPool.h"
#include "RenderingVulkan/VulkanCommandPool.h"
#include "RenderingVulkan/VulkanCommandList.h"
#include "RenderingVulkan/VulkanRenderPass.h"
#include "RenderingVulkan/VulkanFrameBuffer.h"
#include "RenderingVulkan/VulkanRootSignature.h"
#include "RenderingVulkan/VulkanSharedDefines.h"
#include "RenderingVulkan/VulkanRenderTarget.h"

#include <vector>

namespace Raven 
{
	/*! \class RVulkanRenderer
	\brief The RVulkanRenderer class is an vulkan implementation of the rendering interface. 
	*/
	class RVulkanRenderer : public IRenderer 
	{
	public:
		RVulkanRenderer() : mDevice(nullptr), mInitialized(false), mDescriptorPool(nullptr) {}

		// Inherited via IRenderer
		virtual ERenderResult initialize(FRendererInitializationDesc* pDesc) override;
		virtual ERenderResult destroy() override;
		virtual ERenderResult createSwapChain(IPlatformWindow* pNativeWindow, ISwapChain** ppSwapChain) override;
		virtual ERenderResult destroySwapChain(ISwapChain* pSwapChain) override;
		virtual ERenderResult createQueue(IQueue** ppQueue, FQueueCreateDesc* pDesc) override;
		virtual ERenderResult destroyQueue(IQueue* pQueue) override;
		virtual ERenderResult createCommandPool(ICommandPool** ppCmdPool, IQueue* pQueue, FCommandPoolCreateDesc* pDesc) override;
		virtual ERenderResult destroyCommandPool(ICommandPool* pCmdPool) override;
		virtual ERenderResult createCommandList(ICommandList** ppCommandList, ICommandPool* pCmdPool, ECommandListType type = COMMAND_LIST_TYPE_PRIMARY) override;
		virtual ERenderResult createCommandLists(ICommandList** ppCommandLists, uint32_t numCommandLists, ICommandPool* pCmdPool, ECommandListType type = COMMAND_LIST_TYPE_PRIMARY) override;
		virtual ERenderResult destroyCommandList(ICommandPool* pCmdPool, ICommandList* pCommandList) override;
		virtual ERenderResult destroyCommandLists(ICommandPool* pCmdPool, ICommandList* pCommandLists, uint32_t numLists) override;
		virtual ERenderResult createPipeline(IPipeline** ppPipeline, FGraphicsPipelineCreateDesc* pDesc) override;
		virtual ERenderResult createPipeline(IPipeline** ppPipeline, FComputePipelineCreateDesc* pDesc) override;
		virtual ERenderResult destroyPipeline(IPipeline* pPipeline) override;
		virtual ERenderResult createFence(IFence** ppFence, uint64_t value = 1) override;
		virtual ERenderResult destroyFence(IFence* pFence) override;
		virtual ERenderResult createSemaphore(ISemaphore** ppSemaphore) override;
		virtual ERenderResult destroySemaphore(ISemaphore* pSemaphore) override;
		virtual ERenderResult createRootSignature(IRootSignature** ppRootSignature, FRootSignatureCreateDesc* pDesc) override;
		virtual ERenderResult destroyRootSignature(IRootSignature* pRootSignature) override;
		virtual ERenderResult createShader(IShader** ppShader, FShaderCreateDesc* pDesc) override;
		virtual ERenderResult destroyShader(IShader* pShader) override;
		virtual ERenderResult createSampler(ISampler** ppSampler, FSamplerCreateDesc* pDesc) override;
		virtual ERenderResult destroySampler(ISampler* pSampler) override;
		virtual ERenderResult createTexture(ITexture** ppTexture, FTextureCreateDesc* pDesc) override;
		virtual ERenderResult destroyTexture(ITexture* pTexture) override;
		virtual ERenderResult createBuffer(IBuffer** ppBuffer, FBufferCreateDesc* pDesc) override;
		virtual ERenderResult destroyBuffer(IBuffer* pBuffer) override;
		virtual ERenderResult createRenderTarget(IRenderTarget** ppRenderTarget, FRenderTargetCreateDesc* pDesc) override;
		virtual ERenderResult destroyRenderTarget(IRenderTarget* pRenderTarget) override;
		virtual ERenderResult queueSubmit(IQueue* pQueue, uint32_t numCmds, ICommandList** ppCmds, IFence* pSignalFence, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores, uint32_t numSignalSemaphores, ISemaphore** ppSignalSemaphores) override;
		virtual ERenderResult queuePresent(IQueue* pQueue, uint32_t numSwapChains, ISwapChain** ppSwapChains, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores) override;
		virtual ERenderResult swapChainAcquireNextImage(ISwapChain* pSwapChain, ISemaphore* pSignalSemaphores, IFence* pSignalFence) override;
		virtual bool isInitialized() const override;

	protected:
		/*! Create a vulkan instance.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult createVulkanInstance(FRendererInitializationDesc* pDesc);

		/*! Queries which physical devices are available and adds them to the mGpus member variable.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult queryAvailablePhysicalDevices();

		/*! Queries if the surface supports swapchains.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult querySwapchainSupport(FVulkanDevice* pDevice, FVulkanSwapChain* pSwapChain);

		/*! Choose the most suitable swapchain support format and fill the swapchain structure with this surface format.
		\returns On success, this command returns RR_SUCCESS.
		 */
		ERenderResult chooseSwapChainSurfaceFormat(FVulkanSwapChain* pSwapChain);

		/*! Choose the most suitable swapchain present mode and fill the swapchain structure with this present mode.
		\returns On success, this command returns RR_SUCCESS.
		 */
		ERenderResult chooseSwapChainPresentMode(FVulkanSwapChain* pSwapChain);

		/*! Choose the most suitable swapchain extent and fill the swapchain structure with this extent.
		\returns On success, this command returns RR_SUCCESS.
		 */
		ERenderResult chooseSwapChainExtent(FVulkanSwapChain* pSwapChain);

		/*! Create a surface for a window.
		\returns On success, this command returns RR_SUCCESS.
		 * */
		ERenderResult createWindowSurface(IPlatformWindow* window, VkSurfaceKHR* pSurface);

		/*! Pick the most suitable physical device. 
		\param[out] ppDevice ppDevice points to an VkPhysicalDevice handle in which the resulting VkPhysicalDevice is returned.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult pickPhysicalDevice(FVulkanPhysicalDevice** ppDevice);

		/*! Calculate a score for the physical device so we can find the most suitable GPU.
		\param[in] rDevice rDevice points to an FVulkanPhysicalDevice handle which we will use.
		\param[in] rRequiredDeviceExtensions rRequiredDeviceExtensions points to an vector of extensions we need to have support for.
		\returns If the device meets the minimum requirements, it's score will be > 0.
		*/
		uint32_t calculatePhysicalDeviceScore(const FVulkanPhysicalDevice* pDevice, const std::vector<const char*>& rRequiredDeviceExtensions);

		/*! Checks if the device extensions specified in rDeviceExtensions are supported by the physical device.
		\param[in] pDevice pDevice points to an VkPhysicalDevice handle which we will use to check for extension support.
		\param[in] rDeviceExtensions rDeviceExtensions points to an vector of extensions we want to check if they are supported.
		\returns On success, this command returns true.
		*/
		bool hasDeviceExtensionSupport(const FVulkanPhysicalDevice* pDevice, const std::vector<const char*>& rDeviceExtensions);

		/*! Retrieve indices for the queue families of the specified physical device
		\param[in] pDevice pDevice points to an VkPhysicalDevice handle which we will use to retrieve queue family indices from.
		\returns a structure of FQueueFamilyIndices.
		*/
		FQueueFamilyIndices retrieveQueueFamilyIndices(const FVulkanPhysicalDevice* pDevice);

		/*! Retrieve index for a queue that supports the specified flags. Will try to prioritize a queue which only supports that/those types.
		\param[in] pDevice pDevice points to an VkPhysicalDevice handle which we will use to retrieve queue family indices from.
		\param[in] flags flags define which type of queue we want to find it's index from.
		\returns On success, this command returns the queue family index, on failure it will return UINT32_MAX.
		*/
		uint32_t retrieveQueueFamilyIndex(const FVulkanPhysicalDevice* pDevice, VkQueueFlags flags);

		/*! Pick the most suitable logical device.
		\param[in] pPhysicalDevice pPhysicalDevice is a reference to the selected physical device.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult createLogicalDevice(FVulkanPhysicalDevice* pPhysicalDevice);

		/*! Checks if validation layers are supported, if so, add them to the VkInstanceCreateInfo structure.
		\param[in, out] rInfo rInfo is a reference to the VkInstanceCreateInfo structure.
		\param[in] rInstanceLayers rInstanceLayers is a reference to a vector of strings describing which validation layers we want to have enabled.
		\returns On success, this command returns RR_SUCCESS, on failure it will return RR_LAYER_NOT_PRESENT.
		*/
		ERenderResult enableValidationLayers(VkInstanceCreateInfo& rInfo, const std::vector<const char*>& rInstanceLayers);

		/*! Checks if the specified instance extensionss are supported, if so, add them to the VkInstanceCreateInfo structure.
		\param[in, out] rInfo rInfo is a reference to the VkInstanceCreateInfo structure.
		\param[in] rInstanceExtensions rInstanceExtensions is a reference to a vector of strings describing which instance extensions we want to have enabled.
		\returns On success, this command returns RR_SUCCESS, on failure it will return RR_EXTENSION_NOT_PRESENT.
		*/
		ERenderResult enableInstanceExtensions(VkInstanceCreateInfo& rInfo, const std::vector<const char*>& rInstanceExtensions);
		
		/*! Set up debugging callback for validation layer output.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult setupDebugCallback();

		/*! Create the debug report callback extension.
		\param[in] pInstance pInstance is a reference to the VkInstance of this renderer.
		\param[in] pCreateInfo pCreateInfo is a reference to the creation info for the debug callback.
		\param[in] pAllocator pAllocator is a reference to an allocator to use for this extension.
		\param[in] pCallback pCallback is a reference to the callback function to call when a debugging string is outputted.
		\returns On success, this command returns VK_SUCCESS.
		*/
		VkResult createDebugReportCallbackExtension(VkInstance pInstance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);

		/*! Destroy the debug report callback extension.
		\param[in] pInstance pInstance is a reference to the VkInstance of this renderer.
		\param[in] pCallback pCallback is a reference to the callback function to call when a debugging string is outputted.
		\param[in] pAllocator pAllocator is a reference to an allocator to use for this extension.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult destroyDebugReportCallbackExtention(VkInstance pInstance, VkDebugReportCallbackEXT pCallback, const VkAllocationCallbacks* pAllocator);

		/*! Create image view for VkImage.
		\param[in] pImage pImage is a reference to the VkImage this image view should be created for.
		\param[in] format format is the surface format this image view should have.
		\param[in] aspectFlags aspectFlags is a bitflag which describes which aspects can be used for this image view.
		\returns On success, this command returns RR_SUCCESS.
		*/
		VkImageView createImageView(FVulkanTexture* pImage, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectFlags);

		/*! Find suitable memory type with the specified flags
		\param[in] typeFilter typeFilter is a bitflag specifying the required memory type bits (Retrieve from VkMemoryRequirements).
		\param[in] properties properties a bit flag of on which device the memory must reside and if their memory is coherent.
		\returns On success, this command returns RR_SUCCESS.
		*/
		uint32_t findSuitableMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		/*! Create a descriptor pool.
		\param[in] ppDescriptorPool ppDescriptorPool is a reference to the FVulkanDescriptorPool pointer in which the created pool has to be returned.
		\param[in] pDesc pDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult createDescriptorPool(FVulkanDescriptorPool** ppDescriptorPool, FVulkanDescriptorPoolCreateDesc* pDesc);

		/*! Destroy a descriptor pool.
		\param[in] pDescriptorPool pDescriptorPool is a reference to the FVulkanDescriptorPool pointer which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		void destroyDescriptorPool(FVulkanDescriptorPool* pDescriptorPool);

		/*! Create a render pass.
		\param[in] ppDescriptorPool ppDescriptorPool is a reference to the FVulkanDescriptorPool pointer in which the created pool has to be returned.
		\param[in] pDesc pDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult createRenderPass(FVulkanRenderPass** ppRenderPass, FVulkanRenderPassCreateDesc* pDesc);

		/*! Create a frame buffer.
		\param[in] ppFrameBuffer ppFrameBuffer is a reference to the FVulkanFrameBuffer pointer in which the created frame buffer is returned.
		\param[in] pDesc pDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		ERenderResult createFrameBuffer(FVulkanFrameBuffer** ppFrameBuffer, FVulkanFrameBufferCreateDesc* pDesc);

		/*! Destroy a render pass.
		\param[in] pRenderPass pRenderPass is a reference to the VkRenderPass pointer which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		void destroyRenderPass(FVulkanRenderPass* pRenderPass);

		// Global variables
		static const uint32_t gDescriptorPoolSize = 4096;
		static const uint32_t gDescriptorSets = gDescriptorPoolSize * VK_DESCRIPTOR_TYPE_RANGE_SIZE;

		VkDescriptorPoolSize gDescriptorPoolSizes[VK_DESCRIPTOR_TYPE_RANGE_SIZE] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, gDescriptorPoolSize },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, gDescriptorPoolSize * 4 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, gDescriptorPoolSize * 4 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1, },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, gDescriptorPoolSize * 4 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, gDescriptorPoolSize * 4 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, gDescriptorPoolSize },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, gDescriptorPoolSize },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1 },
		};

        // Renderer variables.
		VkInstance								mInstance; /*!< Vulkan instance for this renderer. */
		VkDebugReportCallbackEXT				mDebuggingCallback; // callback extension for validation layers.
		std::vector<FVulkanPhysicalDevice>		mGPUs; /*!< Logical device associated with this renderer. */
		FVulkanDevice*							mDevice; /*!< Logical device associated with this renderer. */
		FRendererInitializationDesc				mRendererInfo; /*!< Copy of renderer initialization description to check for certain features. */
		bool									mInitialized; /*!< Boolean to store if the renderer is initialized. */
		FVulkanDescriptorPool*					mDescriptorPool; /*!< Global descriptor pool. */

};
}