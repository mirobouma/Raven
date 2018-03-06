#pragma once
#if defined(PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(PLATFORM_LINUX)
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.h>
#include <vector>

#define GET_VK_FUNC(I, T) (PFN_ ## T)vkGetInstanceProcAddr(I, #T)

namespace Raven {
	// Test class for vulkan rendering until proper interface is set up
	class RVulkanTestRenderer {
	public:
		RVulkanTestRenderer() : physicalDevice(VK_NULL_HANDLE), instance(VK_NULL_HANDLE), device(VK_NULL_HANDLE), graphicsQueue(VK_NULL_HANDLE), presentQueue(VK_NULL_HANDLE), surface(VK_NULL_HANDLE) {}
		~RVulkanTestRenderer() = default;

		void Initialize();

		void Render();
		void Destroy();

		bool CreateInstance();
		bool CheckValidationLayerSupport();
		bool EnableValidationLayers(VkInstanceCreateInfo& info, const std::vector<const char*>& instanceLayers);
		bool EnableExtensions(VkInstanceCreateInfo& info, const std::vector<const char*>& instanceExtensions);
		bool PickPhysicalDevice();
		bool IsDeviceSuitable(const VkPhysicalDevice& device);
		bool CreateLogicalDevice();
		void CreateSurface(void* nativeHandle);

		int FindGraphicsQueueFamily(const VkPhysicalDevice& device);
		int FindPresentQueueFamily(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

		bool SetupDebugCallback();

	private:
		VkInstance instance; // instance of the vulkan api.
		VkDebugReportCallbackEXT callback; // callback extension for validation layers.
		VkPhysicalDevice physicalDevice; // physical device.
		VkDevice device; // logical device.
		VkQueue graphicsQueue; // graphics queue.
		VkQueue presentQueue; // present queue.
		VkSurfaceKHR surface; // surface.

		bool enableValidationLayers;
	};
}