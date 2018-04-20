#pragma once
#include "Rendering/Interfaces/IRenderer.h"
#include "RenderingVulkan/VulkanDevice.h"
#include "RenderingVulkan/VulkanSwapChain.h"
#include "RenderingVulkan/Vulkan.h"
#include <vector>

namespace Raven 
{
	/*! \class RVulkanRenderer
	\brief The RVulkanRenderer class is an vulkan implementation of the rendering interface. 
	*/
	class RVulkanRenderer : public IRenderer 
	{
	public:
		// Inherited via IRenderer
		virtual ERenderResult initialize(FRendererInitializationDesc* pDesc) override;
		virtual ERenderResult destroy() override;
		virtual ERenderResult createSwapChain(IPlatformWindow* pNativeWindow, ISwapChain** ppSwapChain) override;
		virtual ERenderResult destroySwapChain(ISwapChain* pSwapChain) override;
		virtual ERenderResult createQueue(IQueue** ppQueue, FQueueCreateDesc* pDesc) override;
		virtual ERenderResult destroyQueue(IQueue* pQueue) override;
		virtual ERenderResult createCommandPool(ICommandPool** ppCmdPool, IQueue* pQueue, FCommandPoolCreateDesc* pDesc) override;
		virtual ERenderResult destroyCommandPool(ICommandPool* pCmdPool) override;
		virtual ERenderResult createCommandList(ICommandList** ppCommandList, ICommandPool* pCmdPool) override;
		virtual ERenderResult createCommandLists(ICommandList** ppCommandLists, uint32_t numCommandLists, ICommandPool* pCmdPool) override;
		virtual ERenderResult destroyCommandList(ICommandList* pTexture) override;
		virtual ERenderResult createPipeline(IGraphicsPipeline** ppPipeline, FGraphicsPipelineCreateDesc* pDesc) override;
		virtual ERenderResult destroyPipeline(IGraphicsPipeline* pPipeline) override;
		virtual ERenderResult createPipeline(IComputePipeline** ppPipeline, FComputePipelineCreateDesc* pDesc) override;
		virtual ERenderResult destroyPipeline(IComputePipeline* pPipeline) override;
		virtual ERenderResult createRootSignature(IRootSignature** ppRootSignature, FRootSignatureCreateDesc* pDesc) override;
		virtual ERenderResult destroyRootSignature(IRootSignature* pRootSignature) override;
		virtual ERenderResult createShader(IShader** ppShader, FShaderCreateDesc* pDesc) override;
		virtual ERenderResult destroyShader(IShader* pShader) override;
		virtual ERenderResult createTexture(ITexture** ppTexture, FTextureCreateDesc* pDesc) override;
		virtual ERenderResult destroyTexture(ITexture* pTexture) override;
		virtual ERenderResult createRenderTarget(IRenderTarget** ppRenderTarget, FRenderTargetCreateDesc* pDesc) override;
		virtual ERenderResult destroyRenderTarget(IRenderTarget* pRenderTarget) override;

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

        // Renderer variables.
		VkInstance								mInstance; /*!< Vulkan instance for this renderer. */
		VkDebugReportCallbackEXT				mDebuggingCallback; // callback extension for validation layers.
		std::vector<FVulkanPhysicalDevice>		mGPUs; /*!< Logical device associated with this renderer. */
		FVulkanDevice							mDevice; /*!< Logical device associated with this renderer. */
		bool									mShouldEnableValidationLayers; /*!< Enables validation layers for debugging. */
		bool									mInitialized; /*!< Boolean to store if the renderer is initialized. */

};
}