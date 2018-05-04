#pragma once
#include "Platform/Interfaces/IPlatformWindow.h"
#include <vector>
#include <string>

// Structure interfaces.
#include "Rendering/Interfaces/IBuffer.h"
#include "Rendering/Interfaces/ICommandPool.h"
#include "Rendering/Interfaces/IQueue.h"
#include "Rendering/Interfaces/ICommandList.h"
#include "Rendering/Interfaces/IPipeline.h"
#include "Rendering/Interfaces/IRenderTarget.h"
#include "Rendering/Interfaces/IRootSignature.h"
#include "Rendering/Interfaces/IFence.h"
#include "Rendering/Interfaces/ISemaphore.h"
#include "Rendering/Interfaces/ISampler.h"
#include "Rendering/Interfaces/IShader.h"
#include "Rendering/Interfaces/ISwapChain.h"
#include "Rendering/Interfaces/ITexture.h"

namespace Raven {

	/*! Enumeration of rendering results. */
	typedef enum ERenderResult {
		RR_SUCCESS = 0,
		RR_UNKNOWN_FAILURE = 0,
		RR_ALREADY_INITIALIZED,
		RR_OBJECT_DOES_NOT_EXIST,
		RR_OUT_OF_HOST_MEMORY,
		RR_OUT_OF_DEVICE_MEMORY,
		RR_INITIALIZATION_FAILED,
		RR_DEVICE_LOST,
		RR_MEMORY_MAP_FAILED,
		RR_LAYER_NOT_PRESENT,
		RR_EXTENSION_NOT_PRESENT,
		RR_FEATURE_NOT_PRESENT,
		RR_NO_SUITABLE_DEVICE,
		RR_INCOMPATIBLE_DRIVER,
		RR_TOO_MANY_OBJECTS,
		RR_FORMAT_NOT_SUPPORTED,

	} ERenderResult;

	/*! \struct FRendererInitializationDesc
	\brief Structure holding renderer initialization information.
	*/
	typedef struct FRendererInitializationDesc 
	{
		std::string applicationName;
		uint32_t	versionMajor;
		uint32_t	versionMinor;
		uint32_t	versionPatch;
		bool		enableDebugging;
		std::vector<std::string> requiredVulkanInstanceExtensions;
		std::vector<std::string> requiredVulkanInstanceLayers;
		std::vector<std::string> requiredVulkanDeviceExtensions;
		std::vector<std::string> requiredVulkanDeviceLayers;
	} FRendererInitializationDesc;

	/*! \class IRenderer
	\brief The IRenderer class in an abstract interface for sending rendering commands to the underlying graphics API.
	*/
	class IRenderer 
	{
	public:
		/*! Initialize the renderer. 
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult initialize(FRendererInitializationDesc* pDesc) = 0;

		/*! Destroy the renderer. 
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroy() = 0;

		/*! Create a swapchain for the specified window. 
		\param[in] nativeWindow Reference to the native window instance this swapchain has to be created for.
		\param[in,out] ppSwapChain ppSwapChain points to an ISwapChain handle in which the resulting swapchain is returned.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createSwapChain(IPlatformWindow* pNativeWindow, ISwapChain** ppSwapChain) = 0;

		/*! Destroy a swapchain. 
		\param[in] pSwapChain pSwapChain points to a ISwapChain handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroySwapChain(ISwapChain* pSwapchain) = 0;

		/*! create a command queue.
		\param[in] ppQueue ppQueue points a IQueue handle in which the resulting command queue reference is stored.
		\param[in] pDesc rDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createQueue(IQueue** ppQueue, FQueueCreateDesc* pDesc) = 0;

		/*! Destroy a command queue.
		\param[in] pCmdPool pQueue points to a IQueue handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyQueue(IQueue* pQueue) = 0;

		/*! create a command pool.
		\param[in] ppCmdPool ppCmdPool points a ICommandPool handle in which the resulting command pool reference is stored.
		\param[in] pQueue pQueue points to the command queue this command queue will be created for, these objects share the same command type.
		\param[in] pDesc rDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createCommandPool(ICommandPool** ppCmdPool, IQueue* pQueue, FCommandPoolCreateDesc* pDesc) = 0;

		/*! Destroy a command pool.
		\param[in] pCmdPool pCmdPool is a ICommandPool handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyCommandPool(ICommandPool* pCmdPool) = 0;

		/*! create a command list.
		\param[in] ppCommandList ppCommandList points a ICommandList handle in which the resulting command list is stored.
		\param[in] pCmdPool pCmdPool points to the command pool in which the command list should be allocated.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createCommandList(ICommandList** ppCommandList, ICommandPool* pCmdPool, ECommandListType type = COMMAND_LIST_TYPE_PRIMARY) = 0;

		/*! create multiple command lists.
		\param[in] ppCommandList ppCommandList points an array of ICommandList handles in which the resulting command list is stored.
		\param[in] numCommandLists amount of command lists to create.
		\param[in] pCmdPool pCmdPool points to the command pool in which the command list should be allocated.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createCommandLists(ICommandList** ppCommandLists, uint32_t numCommandLists, ICommandPool* pCmdPool, ECommandListType type = COMMAND_LIST_TYPE_PRIMARY) = 0;

		/*! Destroy a command list.
		\param[in] pCmdPool pCmdPool is a ICommandPool handle which this command belongs to.
		\param[in] pCommandList pCommandList is a ICommandList handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyCommandList(ICommandPool* pCmdPool, ICommandList* pCommandList) = 0;

		/*! create a buffer.
		\param[in] ppBuffer ppBuffer points a IBuffer handle in which the resulting buffer object reference is stored.
		\param[in] pDesc pDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createBuffer(IBuffer** ppBuffer, FBufferCreateDesc* pDesc) = 0;

		/*! Destroy a buffer.
		\param[in] pBuffer pBuffer is a IBuffer handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyBuffer(IBuffer* pBuffer) = 0;

		/*! create a fence.
		\param[in] ppFence ppFence points a IFence handle in which the resulting fence object reference is stored.
		\param[in] value value is the starting value this fence should have.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createFence(IFence** ppFence, uint64_t value = 1) = 0;

		/*! Destroy a fence.
		\param[in] pFence pFence is a IFence handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyFence(IFence* pFence) = 0;

		/*! create a semaphore.
		\param[in] ppSemaphore ppSemaphore points a ISemaphore handle in which the resulting semaphore object reference is stored.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createSemaphore(ISemaphore** ppSemaphore) = 0;

		/*! Destroy a semaphore.
		\param[in] pSemaphore pSemaphore is a ISemaphore handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroySemaphore(ISemaphore* pSemaphore) = 0;

		/*! create a sampler.
		\param[in] ppSampler ppSampler points a ISampler handle in which the resulting buffer object reference is stored.
		\param[in] pDesc pDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createSampler(ISampler** ppSampler, FSamplerCreateDesc* pDesc) = 0;

		/*! Destroy a sampler.
		\param[in] pSampler pSampler is a ISampler handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroySampler(ISampler* pSampler) = 0;

		/*! Destroy multiple command lists.
		\param[in] pCmdPool pCmdPool is a ICommandPool handle which this command belongs to.
		\param[in] pCommandList pCommandList is a ICommandList handle to the first list that has to be destroyed.
		\param[in] numLists numLists is a the amount of commands lists to destroy.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyCommandLists(ICommandPool* pCmdPool, ICommandList* pCommandLists, uint32_t numLists) = 0;

		/*! create a graphics pipeline.
		\param[in] ppPipeline ppPipeline points a IGraphicsPipeline handle in which the resulting graphics pipeline is stored.
		\param[in] pDesc rDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createPipeline(IPipeline** ppPipeline, FGraphicsPipelineCreateDesc* pDesc) = 0;


		/*! create a compute pipeline.
		\param[in] ppPipeline ppPipeline points a IComputePipeline handle in which the resulting compute pipeline is stored.
		\param[in] pDesc rDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createPipeline(IPipeline** ppPipeline, FComputePipelineCreateDesc* pDesc) = 0;

		/*! Destroy a pipeline.
		\param[in] pPipeline pPipeline is a IPipeline handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyPipeline(IPipeline* pPipeline) = 0;

		/*! create a root signature.
		\param[in] ppRootSignature ppRootSignature points a IRootSignature handle in which the resulting root signature is stored.
		\param[in] pDesc rDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createRootSignature(IRootSignature** ppRootSignature, FRootSignatureCreateDesc* pDesc) = 0;

		/*! Destroy a root signature.
		\param[in] pRootSignature pRootSignature is a IRootSignature handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyRootSignature(IRootSignature* pRootSignature) = 0;

		/*! create a shader.
		\param[in] ppShader ppShader points a IShader handle in which the resulting shader is stored.
		\param[in] pDesc rDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createShader(IShader** ppShader, FShaderCreateDesc* pDesc) = 0;

		/*! Destroy a shader.
		\param[in] pShader pShader is a IShader handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyShader(IShader* pShader) = 0;

		/*! create a texture.
		\param[in] ppTexture ppTexture points a ITexture handle in which the resulting texture is stored.
		\param[in] pDesc rDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createTexture(ITexture** ppTexture, FTextureCreateDesc* pDesc) = 0;

		/*! Destroy a texture.
		\param[in] pTexture pTexture is a ITexture handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyTexture(ITexture* pTexture) = 0;

		/*! create a render target.
		\param[in] ppRenderTarget ppRenderTarget points a IRenderTarget handle in which the resulting render target is stored.
		\param[in] pDesc rDesc points to a structure describing additional creation info.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult createRenderTarget(IRenderTarget** ppRenderTarget, FRenderTargetCreateDesc* pDesc) = 0;

		/*! Destroy a render target.
		\param[in] pRenderTarget pRenderTarget is a IRenderTarget handle which has to be destroyed.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult destroyRenderTarget(IRenderTarget* pRenderTarget) = 0;

		/*! Submit command lists to a queue.
		\param[in] pQueue Pointer to the queue in which the command lists have to be executed.
		\param[in] numCmds Amount of command lists to submit to the queue.
		\param[in] ppCmds Pointer to a list of command lists.
		\param[in] pSignalFence Pointer to a fence which has to be signaled once the work has been completed.
		\param[in] numWaitSemaphores Amount of semaphores this submission has to wait for.
		\param[in] ppWaitSemaphores Pointer to a list of semaphores we have to wait for before execution.
		\param[in] numSignalSemaphores Amount of semaphores to signal once completion is finished.
		\param[in] ppSignalSemaphores Pointer to a list of semaphores which have to be signaled on completion.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult queueSubmit(IQueue* pQueue, uint32_t numCmds, ICommandList** ppCmds, IFence* pSignalFence, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores, uint32_t numSignalSemaphores, ISemaphore** ppSignalSemaphores) = 0;

		/*! Present an image to one or multiple swapchains.
		\param[in] pQueue Pointer to the queue in which the present command has to be executed.
		\param[in] numSwapChains Amount of swapchains to present.
		\param[in] ppSwapChains Pointer to a list of swapchains.
		\param[in] numWaitSemaphores Amount of semaphores this submission has to wait for.
		\param[in] ppWaitSemaphores Pointer to a list of semaphores we have to wait for before execution.
		\param[in] pImageIndices List of image indices to present to for each swapchain we present.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult queuePresent(IQueue* pQueue, uint32_t numSwapChains, ISwapChain** ppSwapChains, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores) = 0;

		/*! Acquire next image on the swapchain.
		\param[in] pSwapChain Pointer to the swapchain we have to retrieve the next image index from.
		\param[in] pSignalSemaphores pointer to a semaphore we have to signal once we have acquired the next image.
		\param[in] pSignalFence pointer to a fence we have to signal once we have acquired the next image.
		\returns On success, this command returns RR_SUCCESS.
		*/
		virtual ERenderResult swapChainAcquireNextImage(ISwapChain* pSwapChain, ISemaphore* pSignalSemaphores, IFence* pSignalFence) = 0;

		/*! Check if the renderer has been initialized.
		\returns If initialized, this command will return true.
		*/
		virtual bool isInitialized() const = 0;
	};
}