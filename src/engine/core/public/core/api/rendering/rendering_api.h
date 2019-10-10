#pragma once
#include "core/vector.h"
#include "core/string.h"
#include "math/rect.h"

// Structure interfaces.
#include "core/api/rendering/buffer.h"
#include "core/api/rendering/command_pool.h"
#include "core/api/rendering/queue.h"
#include "core/api/rendering/command_list.h"
#include "core/api/rendering/pipeline.h"
#include "core/api/rendering/render_target.h"
#include "core/api/rendering/root_signature.h"
#include "core/api/rendering/fence.h"
#include "core/api/rendering/semaphore.h"
#include "core/api/rendering/sampler.h"
#include "core/api/rendering/shader.h"
#include "core/api/rendering/swap_chain.h"
#include "core/api/rendering/image.h"
#include "core/os/window.h"

/*! Enumeration of rendering results. */
typedef enum RenderResult {
    RR_FAIL = 0,
    RR_SUCCESS = 1,
    RR_UNKNOWN_FAILURE,
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

} RenderResult;


typedef struct RendererInitializationDesc
{
    String          applicationName;
    uint32_t	    versionMajor;
    uint32_t	    versionMinor;
    uint32_t	    versionPatch;
    bool		    enableDebugging;
    Vector<String>  requiredVulkanInstanceExtensions;
    Vector<String>  requiredVulkanInstanceLayers;
    Vector<String>  requiredVulkanDeviceExtensions;
    Vector<String>  requiredVulkanDeviceLayers;
} RendererInitializationDesc;

class IRenderingAPI
{
public:
    /*! Initialize the renderer.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult initialize(RendererInitializationDesc* pDesc) = 0;

    /*! Destroy the renderer.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy() = 0;

    /*! Create a swapchain for the specified window.
    \param[in] nativeWindow Reference to the native window instance this swapchain has to be created for.
    \param[in,out] ppSwapChain ppSwapChain points to an ISwapChain handle in which the resulting swapchain is returned.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_swap_chain(Window* pNativeWindow, ISwapChain** ppSwapChain) = 0;

    /*! Destroy a swapchain.
    \param[in] pSwapChain pSwapChain points to a ISwapChain handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_swap_chain(ISwapChain* pSwapchain) = 0;

    /*! Recreate a swapchain.
    \param[in] pSwapChain pSwapChain points to a ISwapChain handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult recreate_swap_chain(ISwapChain* pSwapchain) = 0;

    /*! create a command queue.
    \param[in] ppQueue ppQueue points a IQueue handle in which the resulting command queue reference is stored.
    \param[in] pDesc rDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_queue(IQueue** ppQueue, QueueCreateDesc* pDesc) = 0;

    /*! Destroy a command queue.
    \param[in] pCmdPool pQueue points to a IQueue handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_queue(IQueue* pQueue) = 0;

    /*! create a command pool.
    \param[in] ppCmdPool ppCmdPool points a ICommandPool handle in which the resulting command pool reference is stored.
    \param[in] pQueue pQueue points to the command queue this command queue will be created for, these objects share the same command type.
    \param[in] pDesc rDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_command_pool(ICommandPool** ppCmdPool, IQueue* pQueue, CommandPoolCreateDesc* pDesc) = 0;

    /*! Destroy a command pool.
    \param[in] pCmdPool pCmdPool is a ICommandPool handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_command_pool(ICommandPool* pCmdPool) = 0;
                 
    /*! create a command list.
    \param[in] ppCommandList ppCommandList points a ICommandList handle in which the resulting command list is stored.
    \param[in] pCmdPool pCmdPool points to the command pool in which the command list should be allocated.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_command_list(ICommandList** ppCommandList, ICommandPool* pCmdPool, CommandListType type = COMMAND_LIST_TYPE_PRIMARY) = 0;

    /*! create multiple command lists.
    \param[in] ppCommandList ppCommandList points an array of ICommandList handles in which the resulting command list is stored.
    \param[in] numCommandLists amount of command lists to create.
    \param[in] pCmdPool pCmdPool points to the command pool in which the command list should be allocated.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_command_lists(ICommandList** ppCommandLists, uint32_t numCommandLists, ICommandPool* pCmdPool, CommandListType type = COMMAND_LIST_TYPE_PRIMARY) = 0;

    /*! Destroy a command list.
    \param[in] pCmdPool pCmdPool is a ICommandPool handle which this command belongs to.
    \param[in] pCommandList pCommandList is a ICommandList handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_command_list(ICommandPool* pCmdPool, ICommandList* pCommandList) = 0;

    /*! create a buffer.
    \param[in] ppBuffer ppBuffer points a IBuffer handle in which the resulting buffer object reference is stored.
    \param[in] pDesc pDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_buffer(IBuffer** ppBuffer, BufferCreateDesc* pDesc) = 0;

    /*! Destroy a buffer.
    \param[in] pBuffer pBuffer is a IBuffer handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_buffer(IBuffer* pBuffer) = 0;

    /*! create a fence.
    \param[in] ppFence ppFence points a IFence handle in which the resulting fence object reference is stored.
    \param[in] value value is the starting value this fence should have.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_fence(IFence** ppFence, uint64_t value = 1) = 0;

    /*! Destroy a fence.
    \param[in] pFence pFence is a IFence handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_fence(IFence* pFence) = 0;

    /*! Wait for fence
    \param[in] pFence pFence is a IFence handle which we have to wait for.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult wait_for_fence(IFence* pFence) = 0;

    /*! create a semaphore.
    \param[in] ppSemaphore ppSemaphore points a ISemaphore handle in which the resulting semaphore object reference is stored.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_semaphore(ISemaphore** ppSemaphore) = 0;

    /*! Destroy a semaphore.
    \param[in] pSemaphore pSemaphore is a ISemaphore handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_semaphore(ISemaphore* pSemaphore) = 0;

    /*! create a sampler.
    \param[in] ppSampler ppSampler points a ISampler handle in which the resulting buffer object reference is stored.
    \param[in] pDesc pDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_sampler(ISampler** ppSampler, SamplerCreateDesc* pDesc) = 0;

    /*! Destroy a sampler.
    \param[in] pSampler pSampler is a ISampler handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_sampler(ISampler* pSampler) = 0;

    /*! Destroy multiple command lists.
    \param[in] pCmdPool pCmdPool is a ICommandPool handle which this command belongs to.
    \param[in] pCommandList pCommandList is a ICommandList handle to the first list that has to be destroyed.
    \param[in] numLists numLists is a the amount of commands lists to destroy.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_command_lists(ICommandPool* pCmdPool, ICommandList** pCommandLists, uint32_t numLists) = 0;

    /*! create a graphics pipeline.
    \param[in] ppPipeline ppPipeline points a IGraphicsPipeline handle in which the resulting graphics pipeline is stored.
    \param[in] pDesc rDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_pipeline(IPipeline** ppPipeline, GraphicsPipelineCreateDesc* pDesc) = 0;


    /*! create a compute pipeline.
    \param[in] ppPipeline ppPipeline points a IComputePipeline handle in which the resulting compute pipeline is stored.
    \param[in] pDesc rDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_pipeline(IPipeline** ppPipeline, ComputePipelineCreateDesc* pDesc) = 0;

    /*! Destroy a pipeline.
    \param[in] pPipeline pPipeline is a IPipeline handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_pipeline(IPipeline* pPipeline) = 0;

    /*! create a root signature.
    \param[in] ppRootSignature ppRootSignature points a IRootSignature handle in which the resulting root signature is stored.
    \param[in] pDesc rDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_root_signature(IRootSignature** ppRootSignature, RootSignatureCreateDesc* pDesc) = 0;

    /*! Destroy a root signature.
    \param[in] pRootSignature pRootSignature is a IRootSignature handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_root_signature(IRootSignature* pRootSignature) = 0;

    /*! create a shader.
    \param[in] ppShader ppShader points a IShader handle in which the resulting shader is stored.
    \param[in] pDesc rDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_shader(IShader** ppShader, ShaderCreateDesc* pDesc) = 0;

    /*! Destroy a shader.
    \param[in] pShader pShader is a IShader handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_shader(IShader* pShader) = 0;

    /*! create a image.
    \param[in] ppImage ppImage points a IImage handle in which the resulting image is stored.
    \param[in] pDesc rDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_image(IImage** ppImage, ImageCreateDesc* pDesc) = 0;

    /*! Destroy a image.
    \param[in] pImage pImage is a IImage handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_image(IImage* pImage) = 0;

    /*! create a render target.
    \param[in] ppRenderTarget ppRenderTarget points a IRenderTarget handle in which the resulting render target is stored.
    \param[in] pDesc rDesc points to a structure describing additional creation info.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult create_render_target(IRenderTarget** ppRenderTarget, RenderTargetCreateDesc* pDesc) = 0;

    /*! Destroy a render target.
    \param[in] pRenderTarget pRenderTarget is a IRenderTarget handle which has to be destroyed.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult destroy_render_target(IRenderTarget* pRenderTarget) = 0;

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
    virtual RenderResult queue_submit(IQueue* pQueue, uint32_t numCmds, ICommandList** ppCmds, IFence* pSignalFence, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores, uint32_t numSignalSemaphores, ISemaphore** ppSignalSemaphores) = 0;

    /*! Present an image to one or multiple swapchains.
    \param[in] pQueue Pointer to the queue in which the present command has to be executed.
    \param[in] numSwapChains Amount of swapchains to present.
    \param[in] ppSwapChains Pointer to a list of swapchains.
    \param[in] numWaitSemaphores Amount of semaphores this submission has to wait for.
    \param[in] ppWaitSemaphores Pointer to a list of semaphores we have to wait for before execution.
    \param[in] pImageIndices List of image indices to present to for each swapchain we present.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult queue_present(IQueue* pQueue, uint32_t numSwapChains, ISwapChain** ppSwapChains, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores) = 0;

    /*! Waits for all current work in a queue to complete.
    \param[in] pQueue Pointer to the queue to wait for.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult wait_for_queue(IQueue* pQueue) = 0;


    /*! Waits for all current work in a device to complete.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult wait_for_device() = 0;

    /*! Acquire next image on the swapchain.
    \param[in] pSwapChain Pointer to the swapchain we have to retrieve the next image index from.
    \param[in] pSignalSemaphores pointer to a semaphore we have to signal once we have acquired the next image.
    \param[in] pSignalFence pointer to a fence we have to signal once we have acquired the next image.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult swap_chain_acquire_next_image(ISwapChain* pSwapChain, ISemaphore* pSignalSemaphores, IFence* pSignalFence) = 0;

    /*! Begin command list recording.
    \param[in] pCommandList pCommandList points to the command list in which we want to start recoding commands.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_begin(ICommandList* pCommandList) = 0;

    /*! End command list recording.
    \param[in] pCommandList pCommandList points to the command list in which we want to stop recoding commands.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_end(ICommandList* pCommandList) = 0;

    /*! Begin command list recording.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_begin_render_pass(ICommandList* pCommandList, uint32_t numColorAttachments, IRenderTarget** ppColorAttachments, IRenderTarget* pDepthStencilAttachment, LoadStoreOps* pLoadStoreOps) = 0;

    /*! End command list recording.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_end_render_pass(ICommandList* pCommandList) = 0;

    /*! Bind vertex buffer to the pipeline.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_bind_vertex_buffer(ICommandList* pCommandList, uint32_t numBuffers, IBuffer** ppBuffers, uint32_t startSlot = 0) = 0;

    /*! Bind index buffer to the pipeline.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_bind_index_buffer(ICommandList* pCommandList, IBuffer* pBuffer, IndexType type = INDEX_TYPE_UINT16) = 0;

    /*! Adds push constants to the currently bound pipeline
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_push_constant(ICommandList* pCommandList, ShaderStageFlags stages, uint32_t size, uint32_t offset, void* values) = 0;

    /*! Returns a linear allocation for a dynamic uniform buffer.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_allocate_dynamic_uniform_buffer(ICommandList* pCommandList, DynamicUniformAllocation* pAllocation, uint32_t size, void* data) = 0;

    /*! Set viewport for this render pass.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_set_viewport(ICommandList* pCommandList, Rect rect, float minDepth = 0.0f, float maxDepth = 1.0f) = 0;

    /*! Set scissor rects for this render pass.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_set_scissor_rects(ICommandList* pCommandList, Rect rect) = 0;

    /*! Set dynamic depth bias.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_set_dynamic_depth_bias(ICommandList* pCommandList, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;

    /*! bind a pipeline.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_bind_pipeline(ICommandList* pCommandList, IPipeline* pPipeline) = 0;

    /*! Bind index buffer to the pipeline.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_resource_barriers(ICommandList* pCommandList, uint32_t numBufferBarriers, BufferBarrier* pBufferBarriers, uint32_t numImageBarries, ImageBarrier* pImageBarriers, bool flush) = 0;

    /*! Flush all pending barriers into the command list.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_flush_barriers(ICommandList* pCommandList) = 0;

    /*! Draw vertices.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_draw(ICommandList* pCommandList, uint32_t vertexCount, uint32_t vertexOffset) = 0;

    /*! Draw vertices using indices.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_draw_indexed(ICommandList* pCommandList, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;

    /*! Draw vertices using indices and instances.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_draw_instanced(ICommandList* pCommandList, uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount) = 0;

    /*! Draw vertices using indices and instances.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_draw_indexed_instanced(ICommandList* pCommandList, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset, uint32_t instanceCount) = 0;

    /*! Dispatch a compute shader.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_dispatch(ICommandList* pCommandList, uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) = 0;

    /*! Update a resource.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_update_buffer(ICommandList* pCommandList, IBuffer* pDstBuffer, void* data, uint32_t dstOffset, uint32_t size) = 0;

    /*! Update a resource.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_update_image(ICommandList* pCommandList, IImage* pDstImage, void* data, uint32_t width, uint32_t height) = 0;

    /*! Bind descriptors to the currently bound pipeline.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \param[in] numDescriptors numDescriptors is the amount of descriptors to bind from pDescriptors.
    \param[in] pDescriptors pDescriptors points to an array of descriptor bindings to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_bind_descriptors(ICommandList* pCommandList, uint32_t numDescriptors, DescriptorBinding* pDescriptors) = 0;

    /*! Copy the contents of buffer to another.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_copy_buffer(ICommandList* pCommandList, IBuffer* pSrcBuffer, IBuffer* pDstBuffer, uint32_t srcOffset, uint32_t dstOffset, uint32_t size) = 0;

    /*! Copy the contents of buffer to an image.
    \param[in] pCommandList pCommandList points to the command list to use for this command.
    \returns On success, this command returns RR_SUCCESS.
    */
    virtual RenderResult cmd_copy_buffer_to_image(ICommandList* pCommandList, IBuffer* pSrcBuffer, IImage* pDstImage, uint32_t srcOffset, uint32_t width, uint32_t height) = 0;

    /*! Check if the renderer has been initialized.
    \returns If initialized, this command will return true.
    */
    virtual bool is_initialized() const = 0;
};
