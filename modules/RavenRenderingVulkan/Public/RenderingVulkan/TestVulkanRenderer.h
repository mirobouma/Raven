#pragma once
#if defined(PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(PLATFORM_LINUX)
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.h>
#include <vector>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <array>

#define GET_VK_FUNC(I, T) (PFN_ ## T)vkGetInstanceProcAddr(I, #T)

namespace Raven {

	// Simple vertex structure.
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {}; // we have two attributes in our vertex, pos and color.

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos); // the first attribute is our position vector.

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, normal); // the second attribute is our texture co-ord.

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord); // the second attribute is our texture co-ord.

			return attributeDescriptions;
		}
	};

	// uniform buffer.
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	// Test class for vulkan rendering until proper interface is set up
	class RVulkanTestRenderer {
	public:
		RVulkanTestRenderer() : physicalDevice(VK_NULL_HANDLE), instance(VK_NULL_HANDLE), device(VK_NULL_HANDLE), graphicsQueue(VK_NULL_HANDLE), presentQueue(VK_NULL_HANDLE), surface(VK_NULL_HANDLE) {}
		~RVulkanTestRenderer() = default;

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

		void Initialize();

		void Render();
		void Destroy();

		bool CreateInstance();
		bool EnableValidationLayers(VkInstanceCreateInfo& info, const std::vector<const char*>& instanceLayers);
		bool EnableExtensions(VkInstanceCreateInfo& info, const std::vector<const char*>& instanceExtensions);
		bool PickPhysicalDevice();
		bool IsDeviceSuitable(const VkPhysicalDevice& device, const std::vector<const char*>& deviceExtensions);
        bool AreDeviceExtensionsAvailable(const VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);
		bool CreateLogicalDevice();
		void CreateSurface(void* nativeHandle);
        bool CreateSwapchain();
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void CreateSwapchainImageViews();
		bool CreateGraphicsPipeline();
		static std::vector<char> ReadFile(const std::string& filename);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSemaphores();
		void RecreateSwapchain();
		void CleanSwapchain();
		void CreateUniformBuffer();
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CreateDescriptorSetLayout();
		void CreateDescriptorPool();
		void CreateDescriptorSet();
		void CreateTextureImage();
		void CreateTextureImageView();
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateImage(uint32_t width, uint32_t height, VkFormat format, uint32_t mips, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer& buffer);
		int FindGraphicsQueueFamily(const VkPhysicalDevice& device);
		int FindPresentQueueFamily(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void CreateTextureSampler();
		bool SetupDebugCallback();
		void CreateDepthResources();
		void LoadModel();
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		bool HasStencilComponent(VkFormat format);

		// Constant vertex array for a quad.
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

	private:
		VkInstance instance; // instance of the vulkan api.
		VkDebugReportCallbackEXT callback; // callback extension for validation layers.
		VkPhysicalDevice physicalDevice; // physical device.
		VkDevice device; // logical device.
		VkQueue graphicsQueue; // graphics queue.
		VkQueue presentQueue; // present queue.
		VkSurfaceKHR surface; // surface.
		VkSwapchainKHR swapChain; // swapchain.
		std::vector<VkImage> swapChainImages; // swapchain images.
		std::vector<VkImageView> swapChainImageViews; // swapchain image views.
		VkFormat swapChainImageFormat; // swapchain image format.
		VkExtent2D swapChainExtent; // swapchain image size.
		VkShaderModule vertShaderModule; // vertex shader module.
		VkShaderModule fragShaderModule; // fragment shader module.
		VkRenderPass renderPass; // render pass.
		VkDescriptorSetLayout descriptorSetLayout; // descriptor set layout.
		VkPipelineLayout pipelineLayout; // pipeline layout.
		VkPipeline graphicsPipeline; // graphics pipeline.
		std::vector<VkFramebuffer> swapChainFramebuffers; // framebuffers for each swapchain image.
		std::vector<VkCommandBuffer> CommandBuffers; // list of command buffers.
		VkCommandPool commandPool; // command pool.
		VkBuffer vertexBuffer; //vertex buffer.
		VkBuffer indexBuffer; // index buffer.
		VkDeviceMemory vertexBufferMemory; // memory for vertex  buffer.
		VkDeviceMemory indexBufferMemory; // memory for index buffer.
		VkBuffer uniformBuffer; // uniform buffer
		VkDeviceMemory uniformBufferMemory; // memory for uniform buffer.
		VkDescriptorPool descriptorPool; // descriptor pool.
		VkDescriptorSet descriptorSet; // descriptor set for uniform buffer.

		VkImage textureImage; // texture image.
		VkImageView textureImageView; // texture image view.
		VkDeviceMemory textureImageMemory; // memory for texture image.
		VkSampler textureSampler; // sampler to sample texture with ani filtering x16

		VkImage depthImage; // depth stencil image
		VkDeviceMemory depthImageMemory; // memory for depth stencil.
		VkImageView depthImageView; // depth image view.


		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

		bool enableValidationLayers;
	};
}