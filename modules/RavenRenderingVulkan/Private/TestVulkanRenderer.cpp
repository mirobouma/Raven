#include "RenderingVulkan/TestVulkanRenderer.h"
#include <vector>
#include <iostream>

#ifdef PLATFORM_LINUX
#include "SDL.h"
#endif

namespace Raven 
{

	void RVulkanTestRenderer::Initialize()
	{
		// Create vulkan instance with extensions and layers.
		CreateInstance();

		// Set up debugging callback for the validation layers.
		SetupDebugCallback();

		// Find a suitable physical device.
		PickPhysicalDevice();
	}


	bool RVulkanTestRenderer::CreateInstance()
	{
		// Define vulkan application. Useful for driver optimizations (aka useless for small applications)
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Raven Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Raven";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;


		// Vulkan instance creation info, pass the application information as argument.
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = 0;
		createInfo.enabledExtensionCount = 0;

		// Define validation layers we want to have attached to our instance.
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		// Define extensions that we need for vulkan (in this case Surface and Win32/XCB surface extensions)
		std::vector<const char*> instanceExtensions;
		instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

		#if defined(PLATFORM_WINDOWS)
		instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#elif defined(PLATFORM_LINUX)
		instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		#endif

		#ifdef NDEBUG
			enableValidationLayers = false;
		#else
			enableValidationLayers = true;
		#endif

		// Enable debug report extensions if we're running in debug mode.
		if (enableValidationLayers)
		{
			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		// Enable validation layers
		if (enableValidationLayers && !EnableValidationLayers(createInfo, validationLayers))
		{
			throw std::runtime_error("failed to enable validation layers!");
		}

		// Enable extensions
		if (!EnableExtensions(createInfo, instanceExtensions))
		{
			throw std::runtime_error("failed to enable extensions!");
		}

		// Create the vulkan instance using our vulkan create info structure. We don't define an allocator for now.
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vulkan instance!");
		}

		return true;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData)
	{

		std::cerr << "validation layer: " << msg << std::endl;

		return VK_FALSE;
	}

	VkResult CreateDebugReportCallbackEXT(
		VkInstance instance,
		const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
		auto func = GET_VK_FUNC(instance, vkCreateDebugReportCallbackEXT);
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pCallback);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
		auto func = GET_VK_FUNC(instance, vkDestroyDebugReportCallbackEXT);
		if (func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}

	bool RVulkanTestRenderer::SetupDebugCallback()
	{
		#ifndef NDEBUG
			VkDebugReportCallbackCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT ; // only errors and warnings for now.
			createInfo.pfnCallback = debugCallback;

			// Create debug extension.
			if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
				throw std::runtime_error("failed to set up debug callback!");
			}
		#endif

		return true;
	}

	bool RVulkanTestRenderer::EnableValidationLayers(VkInstanceCreateInfo& info, const std::vector<const char*>& instanceLayers)
	{

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		// Retrieve available layers into the availablelayers vector.
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// For each layer that we want enabled, check if we support it.
		for (const char* layerName : instanceLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}


		info.enabledLayerCount = instanceLayers.size();
		info.ppEnabledLayerNames = instanceLayers.data();

		return true;
	}

	bool RVulkanTestRenderer::EnableExtensions(VkInstanceCreateInfo& info, const std::vector<const char*>& instanceExtensions)
	{
		// Query how many extensions are available to us.
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		// Container of extensions.
		std::vector<VkExtensionProperties> extensions(extensionCount);

		// Retrieve the extensions we have available into our extensions vector.
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// For each extension that we want enabled, check if we support it.
		for (const char* extensionName : instanceExtensions) {
			bool extFound = false;

			for (const auto& extProperty : extensions) {
				if (strcmp(extensionName, extProperty.extensionName) == 0) {
					extFound = true;
					break;
				}
			}

			if (!extFound) {
				return false;
			}
		}

		// Add the extensions to the create info structure.
		info.enabledExtensionCount = instanceExtensions.size();
		info.ppEnabledExtensionNames = instanceExtensions.data();

		return true;
	}

	bool RVulkanTestRenderer::PickPhysicalDevice()
	{
		// Query how many physical devices we have.
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		// We don't have any gpu's with vulkan support so we can't proceed.
		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		// Get the list of the physical devices we have.
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// Find the first suitable device.
		for (const auto& device : devices) {
			if (IsDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		return true;
	}

	bool RVulkanTestRenderer::IsDeviceSuitable(const VkPhysicalDevice& device) {

		// Query device properties.
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		// Query device features.
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		// Make sure we have a graphics queue family.
		int graphicsIndex = FindGraphicsQueueFamily(device);

		// If we have a discrete GPU and we support geometry shaders, it is enough for us.
		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader && graphicsIndex != -1;

	}

	bool RVulkanTestRenderer::CreateLogicalDevice()
	{
		// Find the graphics queue index.
		int graphicsIndex = FindGraphicsQueueFamily(physicalDevice);
		int presentIndex = FindPresentQueueFamily(physicalDevice, surface);

		std::vector<VkDeviceQueueCreateInfo> queueInfos;

		float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = graphicsIndex;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
		queueInfos.push_back(graphicsQueueCreateInfo);

		if (presentIndex != graphicsIndex)
		{
			VkDeviceQueueCreateInfo presentQueueCreateInfo = {};
			presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			presentQueueCreateInfo.queueFamilyIndex = presentIndex;
			presentQueueCreateInfo.queueCount = 1;
			presentQueueCreateInfo.pQueuePriorities = &queuePriority;
			queueInfos.push_back(presentQueueCreateInfo);
		}

		// We don't need any specific features for this logical device yet.
		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueInfos.data();
		createInfo.queueCreateInfoCount = queueInfos.size();
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		// Enable the same validation layers for this logical device.
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		// Create the device.
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		// Retrieve our graphics queue.
		vkGetDeviceQueue(device, graphicsIndex, 0, &graphicsQueue);
		vkGetDeviceQueue(device, presentIndex, 0, &presentQueue);

		return true;
	}

	int RVulkanTestRenderer::FindGraphicsQueueFamily(const VkPhysicalDevice& device)
	{
		int familyIndex = -1;

		// Get amount of available queue families.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		// Get the list of queue families.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		// Find the graphics queue family.
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				familyIndex = i;
			}

			if (familyIndex != -1) {
				break;
			}

			i++;
		}

		return familyIndex;
	}

	int RVulkanTestRenderer::FindPresentQueueFamily(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
	{
		int familyIndex = -1;

		// Get amount of available queue families.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		// Get the list of queue families.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


		// Find the graphics queue family.
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport) {
				familyIndex = i;
			}

			if (familyIndex != -1) {
				break;
			}

			i++;
		}

		return familyIndex;
	}

	void RVulkanTestRenderer::CreateSurface(void* nativeHandle)
	{
#if defined(PLATFORM_WINDOWS)
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = (HWND)nativeHandle;
		createInfo.hinstance = GetModuleHandle(nullptr);

		auto CreateWin32SurfaceKHR = GET_VK_FUNC(instance, vkCreateWin32SurfaceKHR);

		if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}

#elif defined(PLATFORM_LINUX)
		SDL_SysWMinfo wminfo;
		SDL_VERSION(&wminfo.version);
		if (!SDL_GetWindowWMInfo((SDL_Window*)nativeHandle, &wminfo))
			return;

		if (wminfo.subsystem != SDL_SYSWM_X11)
		{
			throw std::runtime_error("We can only initialize an XCB surface!");
		}

		VkXcbSurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		createInfo.window = wminfo.info.x11.window;
		createInfo.connection = XGetXCBConnection(wminfo.info.x11.display);;

		auto CreateXcbSurfaceKHR = GET_VK_FUNC(instance, vkCreateXcbSurfaceKHR);

		if (!CreateXcbSurfaceKHR || CreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
#endif

		if (device == VK_NULL_HANDLE)
		{
			// Create logical device.
			CreateLogicalDevice();
		}
	}
	void RVulkanTestRenderer::Destroy()
	{
		#ifndef NDEBUG
			// Destroy debugging report callback.
			DestroyDebugReportCallbackEXT(instance, callback, nullptr);
		#endif

		// Destroy our logical device.
		vkDestroyDevice(device, nullptr);

		// Destroy our surface.
		vkDestroySurfaceKHR(instance, surface, nullptr);

		// Destroy our vulkan instance.
		vkDestroyInstance(instance, nullptr);
	}

	void RVulkanTestRenderer::Render()
	{

	}
	
}

