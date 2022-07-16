#pragma once

#include "rendering/rendering_api.h"
#include "renderer-vulkan/vulkan.h"
#include "renderer-vulkan/vulkan_device.h"
#include "renderer-vulkan/vulkan_swap_chain.h"
#include "renderer-vulkan/vulkan_pipeline.h"
#include "renderer-vulkan/vulkan_image.h"
#include "renderer-vulkan/vulkan_buffer.h"
#include "renderer-vulkan/vulkan_queue.h"
#include "renderer-vulkan/vulkan_shader.h"
#include "renderer-vulkan/vulkan_fence.h"
#include "renderer-vulkan/vulkan_semaphore.h"
#include "renderer-vulkan/vulkan_descriptor_pool.h"
#include "renderer-vulkan/vulkan_dynamic_uniform_allocator.h"
#include "renderer-vulkan/vulkan_command_pool.h"
#include "renderer-vulkan/vulkan_command_list.h"
#include "renderer-vulkan/vulkan_render_pass.h"
#include "renderer-vulkan/vulkan_frame_buffer.h"
#include "renderer-vulkan/vulkan_root_signature.h"
#include "renderer-vulkan/vulkan_defines.h"
#include "renderer-vulkan/vulkan_render_target.h"

namespace Raven
{
    /*! \class VulkanRenderer
    \brief The VulkanRenderer class is an vulkan implementation of the rendering interface.
    */
    class VulkanRenderer : public IRenderingAPI
    {
    public:
        VulkanRenderer() : _device(nullptr), _initialized(false), _current_frame_index(0), _allocator(nullptr),
            _debugging_callback(false), _graphics_command_pool(nullptr), _graphics_queue(nullptr), _instance(nullptr),
            _transfer_command_pool(nullptr), _transfer_queue(nullptr) {}

        // Inherited via IRenderInterface
        virtual RenderResult initialize(RendererInitializationDesc* pDesc) override;
        virtual RenderResult destroy() override;
        virtual RenderResult create_swap_chain(INativeWindow* pNativeWindow, ISwapChain** ppSwapChain) override;
        virtual RenderResult destroy_swap_chain(ISwapChain* pSwapChain) override;
        virtual RenderResult recreate_swap_chain(ISwapChain* pSwapChain) override;
        virtual RenderResult create_queue(IQueue** ppQueue, QueueCreateDesc* pDesc) override;
        virtual RenderResult destroy_queue(IQueue* pQueue) override;
        virtual RenderResult create_command_pool(ICommandPool** ppCmdPool, IQueue* pQueue, CommandPoolCreateDesc* pDesc) override;
        virtual RenderResult destroy_command_pool(ICommandPool* pCmdPool) override;
        virtual RenderResult create_command_list(ICommandList** ppCommandList, ICommandPool* pCmdPool, CommandListType type = COMMAND_LIST_TYPE_PRIMARY) override;
        virtual RenderResult create_command_lists(ICommandList** ppCommandLists, uint32_t numCommandLists, ICommandPool* pCmdPool, CommandListType type = COMMAND_LIST_TYPE_PRIMARY) override;
        virtual RenderResult destroy_command_list(ICommandPool* pCmdPool, ICommandList* pCommandList) override;
        virtual RenderResult destroy_command_lists(ICommandPool* pCmdPool, ICommandList** pCommandLists, uint32_t numLists) override;
        virtual RenderResult create_pipeline(IPipeline** ppPipeline, GraphicsPipelineCreateDesc* pDesc) override;
        virtual RenderResult create_pipeline(IPipeline** ppPipeline, ComputePipelineCreateDesc* pDesc) override;
        virtual RenderResult destroy_pipeline(IPipeline* pPipeline) override;
        virtual RenderResult create_fence(IFence** ppFence, uint64_t value = 1) override;
        virtual RenderResult destroy_fence(IFence* pFence) override;
        virtual RenderResult wait_for_fence(IFence* pFence) override;
        virtual RenderResult create_semaphore(ISemaphore** ppSemaphore) override;
        virtual RenderResult destroy_semaphore(ISemaphore* pSemaphore) override;
        virtual RenderResult create_root_signature(IRootSignature** ppRootSignature, RootSignatureCreateDesc* pDesc) override;
        virtual RenderResult destroy_root_signature(IRootSignature* pRootSignature) override;
        virtual RenderResult create_shader(IShader** ppShader, ShaderCreateDesc* pDesc) override;
        virtual RenderResult destroy_shader(IShader* pShader) override;
        virtual RenderResult create_sampler(ISampler** ppSampler, SamplerCreateDesc* pDesc) override;
        virtual RenderResult destroy_sampler(ISampler* pSampler) override;
        virtual RenderResult create_image(IImage** ppTexture, ImageCreateDesc* pDesc) override;
        virtual RenderResult destroy_image(IImage* pTexture) override;
        virtual RenderResult create_buffer(IBuffer** ppBuffer, BufferCreateDesc* pDesc) override;
        virtual RenderResult destroy_buffer(IBuffer* pBuffer) override;
        virtual RenderResult cmd_copy_buffer(ICommandList* pCommandList, IBuffer* pSrcBuffer, IBuffer* pDstBuffer, uint32_t srcOffset, uint32_t dstOffset, uint32_t size) override;
        virtual RenderResult cmd_copy_buffer_to_image(ICommandList* pCommandList, IBuffer* pSrcBuffer, IImage* pDstImage, uint32_t srcOffset, uint32_t width, uint32_t height) override;
        virtual RenderResult cmd_update_buffer(ICommandList* pCommandList, IBuffer* pDstBuffer, void* data, uint32_t dstOffset, uint32_t size) override;
        virtual RenderResult cmd_update_image(ICommandList* pCommandList, IImage* pDstTexture, void* data, uint32_t width, uint32_t height) override;
        virtual RenderResult create_render_target(IRenderTarget** ppRenderTarget, RenderTargetCreateDesc* pDesc) override;
        virtual RenderResult destroy_render_target(IRenderTarget* pRenderTarget) override;
        virtual RenderResult queue_submit(IQueue* pQueue, uint32_t numCmds, ICommandList** ppCmds, IFence* pSignalFence, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores, uint32_t numSignalSemaphores, ISemaphore** ppSignalSemaphores) override;
        virtual RenderResult queue_present(IQueue* pQueue, uint32_t numSwapChains, ISwapChain** ppSwapChains, uint32_t numWaitSemaphores, ISemaphore** ppWaitSemaphores) override;
        virtual RenderResult wait_for_queue(IQueue* pQueue) override;
        virtual RenderResult wait_for_device() override;
        virtual RenderResult swap_chain_acquire_next_image(ISwapChain* pSwapChain, ISemaphore* pSignalSemaphores, IFence* pSignalFence) override;
        virtual RenderResult cmd_begin(ICommandList* pCommandList) override;
        virtual RenderResult cmd_end(ICommandList* pCommandList) override;
        virtual RenderResult cmd_begin_render_pass(ICommandList* pCommandList, uint32_t numColorAttachments, IRenderTarget** ppColorAttachments, IRenderTarget* pDepthStencilAttachment, LoadStoreOps* pLoadStoreOps) override;
        virtual RenderResult cmd_end_render_pass(ICommandList* pCommandList) override;
        virtual RenderResult cmd_allocate_dynamic_uniform_buffer(ICommandList* pCommandList, DynamicUniformAllocation* pAllocation, uint32_t size, void* data) override;
        virtual RenderResult cmd_bind_vertex_buffer(ICommandList* pCommandList, uint32_t numBuffers, IBuffer** ppBuffers, uint32_t startSlot) override;
        virtual RenderResult cmd_bind_index_buffer(ICommandList* pCommandList, IBuffer* pBuffer, IndexType type) override;
        virtual RenderResult cmd_set_viewport(ICommandList* pCommandList, Rect rect, float minDepth, float maxDepth) override;
        virtual RenderResult cmd_set_scissor_rects(ICommandList* pCommandList, Rect rect) override;
        virtual RenderResult cmd_bind_pipeline(ICommandList* pCommandList, IPipeline* pPipeline) override;
        virtual RenderResult cmd_resource_barriers(ICommandList* pCommandList, uint32_t numBufferBarriers, BufferBarrier* pBufferBarriers, uint32_t numTextureBarries, ImageBarrier* pTextureBarriers, bool flush) override;
        virtual RenderResult cmd_flush_barriers(ICommandList* pCommandList) override;
        virtual RenderResult cmd_draw(ICommandList* pCommandList, uint32_t vertexCount, uint32_t vertexOffset) override;
        virtual RenderResult cmd_draw_indexed(ICommandList* pCommandList, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) override;
        virtual RenderResult cmd_draw_instanced(ICommandList* pCommandList, uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount) override;
        virtual RenderResult cmd_draw_indexed_instanced(ICommandList* pCommandList, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset, uint32_t instanceCount) override;
        virtual RenderResult cmd_push_constant(ICommandList* pCommandList, ShaderStageFlags stages, uint32_t size, uint32_t offset, void* values) override;
        virtual RenderResult cmd_dispatch(ICommandList* pCommandList, uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) override;
        virtual RenderResult cmd_bind_descriptors(ICommandList* pCommandList, uint32_t numDescriptors, DescriptorBinding* pDescriptors) override;
        virtual RenderResult cmd_set_dynamic_depth_bias(ICommandList* pCommandList, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override;

        virtual bool is_initialized() const override;

    protected:
        /*! Create a vulkan instance.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult create_vulkan_instance(RendererInitializationDesc* pDesc);

        /*! Queries which physical devices are available and adds them to the mGpus member variable.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult query_available_physical_devices();

        /*! Queries if the surface supports swapchains.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult query_swap_chain_support(VulkanDevice* pDevice, VulkanSwapChain* pSwapChain);

        /*! Choose the most suitable swapchain support format and fill the swapchain structure with this surface format.
        \returns On success, this command returns RR_SUCCESS.
         */
        RenderResult choose_swap_chain_surface_format(VulkanSwapChain* pSwapChain);

        /*! Choose the most suitable swapchain present mode and fill the swapchain structure with this present mode.
        \returns On success, this command returns RR_SUCCESS.
         */
        RenderResult choose_swap_chain_present_mode(VulkanSwapChain* pSwapChain);

        /*! Choose the most suitable swapchain extent and fill the swapchain structure with this extent.
        \returns On success, this command returns RR_SUCCESS.
         */
        RenderResult choose_swap_chain_extent(VulkanSwapChain* pSwapChain);

        /*! Create a surface for a window.
        \returns On success, this command returns RR_SUCCESS.
         * */
        RenderResult create_window_surface(INativeWindow* window, VkSurfaceKHR* pSurface);

        /*! Pick the most suitable physical device.
        \param[out] ppDevice ppDevice points to an VkPhysicalDevice handle in which the resulting VkPhysicalDevice is returned.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult pick_physical_device(VulkanPhysicalDevice** ppDevice);

        /*! Calculate a score for the physical device so we can find the most suitable GPU.
        \param[in] rDevice rDevice points to an FVulkanPhysicalDevice handle which we will use.
        \param[in] rRequiredDeviceExtensions rRequiredDeviceExtensions points to an vector of extensions we need to have support for.
        \returns If the device meets the minimum requirements, it's score will be > 0.
        */
        uint32_t calculate_physical_device_score(const VulkanPhysicalDevice* pDevice, const std::vector<const char*>& rRequiredDeviceExtensions);

        /*! Checks if the device extensions specified in rDeviceExtensions are supported by the physical device.
        \param[in] pDevice pDevice points to an VkPhysicalDevice handle which we will use to check for extension support.
        \param[in] rDeviceExtensions rDeviceExtensions points to an vector of extensions we want to check if they are supported.
        \returns On success, this command returns true.
        */
        bool has_device_extension_support(const VulkanPhysicalDevice* pDevice, const std::vector<const char*>& rDeviceExtensions);

        /*! Retrieve indices for the queue families of the specified physical device
        \param[in] pDevice pDevice points to an VkPhysicalDevice handle which we will use to retrieve queue family indices from.
        \returns a structure of FQueueFamilyIndices.
        */
        QueueFamilyIndices retrieve_queue_family_indices(const VulkanPhysicalDevice* pDevice);

        /*! Retrieve index for a queue that supports the specified flags. Will try to prioritize a queue which only supports that/those types.
        \param[in] pDevice pDevice points to an VkPhysicalDevice handle which we will use to retrieve queue family indices from.
        \param[in] flags flags define which type of queue we want to find it's index from.
        \returns On success, this command returns the queue family index, on failure it will return UINT32_MAX.
        */
        uint32_t retrieve_queue_family_index(const VulkanPhysicalDevice* pDevice, VkQueueFlags flags);

        /*! Pick the most suitable logical device.
        \param[in] pPhysicalDevice pPhysicalDevice is a reference to the selected physical device.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult create_logical_device(VulkanPhysicalDevice* pPhysicalDevice);

        /*! Checks if validation layers are supported, if so, add them to the VkInstanceCreateInfo structure.
        \param[in, out] rInfo rInfo is a reference to the VkInstanceCreateInfo structure.
        \param[in] rInstanceLayers rInstanceLayers is a reference to a vector of strings describing which validation layers we want to have enabled.
        \returns On success, this command returns RR_SUCCESS, on failure it will return RR_LAYER_NOT_PRESENT.
        */
        RenderResult enable_validation_layers(VkInstanceCreateInfo& rInfo, const std::vector<const char*>& rInstanceLayers);

        /*! Checks if the specified instance extensionss are supported, if so, add them to the VkInstanceCreateInfo structure.
        \param[in, out] rInfo rInfo is a reference to the VkInstanceCreateInfo structure.
        \param[in] rInstanceExtensions rInstanceExtensions is a reference to a vector of strings describing which instance extensions we want to have enabled.
        \returns On success, this command returns RR_SUCCESS, on failure it will return RR_EXTENSION_NOT_PRESENT.
        */
        RenderResult enable_instance_extensions(VkInstanceCreateInfo& rInfo, const std::vector<const char*>& rInstanceExtensions);

        /*! Set up debugging callback for validation layer output.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult setup_debug_callback();

        /*! Create the debug report callback extension.
        \param[in] pInstance pInstance is a reference to the VkInstance of this renderer.
        \param[in] pCreateInfo pCreateInfo is a reference to the creation info for the debug callback.
        \param[in] pAllocator pAllocator is a reference to an allocator to use for this extension.
        \param[in] pCallback pCallback is a reference to the callback function to call when a debugging string is outputted.
        \returns On success, this command returns VK_SUCCESS.
        */
        VkResult create_debug_report_callback_extension(VkInstance pInstance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);

        /*! Destroy the debug report callback extension.
        \param[in] pInstance pInstance is a reference to the VkInstance of this renderer.
        \param[in] pCallback pCallback is a reference to the callback function to call when a debugging string is outputted.
        \param[in] pAllocator pAllocator is a reference to an allocator to use for this extension.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult destroy_debug_report_callback_extension(VkInstance pInstance, VkDebugReportCallbackEXT pCallback, const VkAllocationCallbacks* pAllocator);

        /*! Create image view for VkImage.
        \param[in] pImage pImage is a reference to the VkImage this image view should be created for.
        \param[in] format format is the surface format this image view should have.
        \param[in] aspectFlags aspectFlags is a bitflag which describes which aspects can be used for this image view.
        \returns On success, this command returns RR_SUCCESS.
        */
        VkImageView create_image_view(VulkanImage* pImage, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewCreateInfo& desc);

        /*! Find suitable memory type with the specified flags
        \param[in] typeFilter typeFilter is a bitflag specifying the required memory type bits (Retrieve from VkMemoryRequirements).
        \param[in] properties properties a bit flag of on which device the memory must reside and if their memory is coherent.
        \returns On success, this command returns RR_SUCCESS.
        */
        uint32_t find_suitable_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        /*! Create a descriptor pool.
        \param[in] ppDescriptorPool ppDescriptorPool is a reference to the FVulkanDescriptorPool pointer in which the created pool has to be returned.
        \param[in] pDesc pDesc points to a structure describing additional creation info.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult create_descriptor_pool(VulkanDescriptorPool** ppDescriptorPool, VulkanDescriptorPoolCreateDesc* pDesc);

        /*! Destroy a descriptor pool.
        \param[in] pDescriptorPool pDescriptorPool is a reference to the FVulkanDescriptorPool pointer which has to be destroyed.
        \returns On success, this command returns RR_SUCCESS.
        */
        void destroy_descriptor_pool(VulkanDescriptorPool* pDescriptorPool);

        /*! Create a render pass.
        \param[in] ppDescriptorPool ppDescriptorPool is a reference to the FVulkanDescriptorPool pointer in which the created pool has to be returned.
        \param[in] pDesc pDesc points to a structure describing additional creation info.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult create_render_pass(VulkanRenderPass** ppRenderPass, VulkanRenderPassCreateDesc* pDesc);

        /*! Create a frame buffer.
        \param[in] ppFrameBuffer ppFrameBuffer is a reference to the FVulkanFrameBuffer pointer in which the created frame buffer is returned.
        \param[in] pDesc pDesc points to a structure describing additional creation info.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult create_frame_buffer(VulkanFrameBuffer** ppFrameBuffer, VulkanFrameBufferCreateDesc* pDesc);

        /*! Destroy a frame buffer.
        \param[in] pFrameBuffer pFrameBuffer is a reference to the FVulkanFrameBuffer which we have to destroy.
        \returns On success, this command returns RR_SUCCESS.
        */
        RenderResult destroy_frame_buffer(VulkanFrameBuffer* pFrameBuffer);

        /*! Destroy a render pass.
        \param[in] pRenderPass pRenderPass is a reference to the VkRenderPass pointer which has to be destroyed.
        \returns On success, this command returns RR_SUCCESS.
        */
        void destroy_render_pass(VulkanRenderPass* pRenderPass);

        /*! Retrieve a pointer to the frame buffer cache local to this thread.
        \returns pointer to a FVulkanFramebufferCache created for this thread.
        */
        VulkanFramebufferCache* get_thread_local_framebuffer_cache();

        /*! Retrieve a pointer to the descriptor pool local to this thread.
        \returns pointer to a FVulkanDescriptorPool created for this thread.
        */
        VulkanDescriptorPool* get_thread_local_descriptor_pool();

        /*! Retrieve a pointer to the descriptor pool local to this thread.
        \returns pointer to a FVulkanDescriptorPool created for this thread.
        */
        VulkanRenderPassCache* get_thread_local_render_pass_cache();

        /*! Retrieve a pointer to the descriptor pool local to this thread.
        \returns pointer to a FVulkanDescriptorPool created for this thread.
        */
        VulkanDescriptorSetCache* get_thread_local_descriptor_set_cache();

        static const uint32_t VK_DESCRIPTOR_TYPE_RANGE_SIZE = (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT - VK_DESCRIPTOR_TYPE_SAMPLER + 1);

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
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, gDescriptorPoolSize * 4 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, gDescriptorPoolSize },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1 },
        };

        static const uint32_t gDynamicDescriptorPoolSize = gDescriptorPoolSize / 16;
        static const uint32_t gDynamicDescriptorSets = gDynamicDescriptorPoolSize * VK_DESCRIPTOR_TYPE_RANGE_SIZE;

        VkDescriptorPoolSize gDynamicDescriptorPoolSizes[VK_DESCRIPTOR_TYPE_RANGE_SIZE] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, gDynamicDescriptorPoolSize },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, gDynamicDescriptorPoolSize },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, gDynamicDescriptorPoolSize },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1, },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, gDynamicDescriptorPoolSize },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, gDynamicDescriptorPoolSize },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, gDynamicDescriptorPoolSize * 4},
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, gDynamicDescriptorPoolSize },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1 },
        };


        // Renderer variables.
        VkInstance								            _instance; /*!< Vulkan instance for this renderer. */
        VkDebugReportCallbackEXT				            _debugging_callback; // callback extension for validation layers.
        std::vector<VulkanPhysicalDevice>		            _gpus; /*!< Logical device associated with this renderer. */
        VulkanDevice* _device; /*!< Logical device associated with this renderer. */
        VmaAllocator										_allocator; /*!< Vulkan memory allocator. */
        VulkanQueue* _transfer_queue; /*!< Transfer queue for data transfer. */
        VulkanCommandPool* _transfer_command_pool; /*!< Transfer command pool for data transfer. */
        VulkanQueue* _graphics_queue; /*!< Transfer queue for data transfer. */
        VulkanCommandPool* _graphics_command_pool; /*!< Transfer command pool for data transfer. */
        RendererInitializationDesc				            _renderer_info; /*!< Copy of renderer initialization description to check for certain features. */
        bool									            _initialized; /*!< Boolean to store if the renderer is initialized. */

        template <typename T>
        using ThreadLocalMap = std::map<uint64_t, T>;

        ThreadLocalMap<VulkanDescriptorPool*>				_descriptor_pools; /*!< Global descriptor pool. */
        ThreadLocalMap<VulkanDescriptorSetCache*>			_descriptor_set_caches; /*!< Descriptor set cache */
        ThreadLocalMap<VulkanFramebufferCache*>			    _frame_buffer_maps; /*!< Cache of frame buffers. */
        ThreadLocalMap<VulkanRenderPassCache*>	            _render_pass_maps; /*!< Cache of render passes. */

        uint32_t											_current_frame_index;
    };
}