#include "RenderingVulkan/VulkanRenderer.h"
#include "Core/Debug/LogManager.h"
#include <iostream>
#include <set>

namespace Raven 
{
	ERenderResult RVulkanRenderer::Initialize()
	{
		// Don't try to initialize if we already successfully initialized before.
		if (IsInitialized())
		{
			return RR_ALREADY_INITIALIZED;
		}

		// Local variable for storing results of initialization functions.
		ERenderResult result;

		// Create a vulkan instance.
		result = CreateVulkanInstance();
		if (result != RR_SUCCESS)
		{
			return result;
		}

		// Set up debugging callbacks
		if (mShouldEnableValidationLayers)
		{
			result = SetupDebugCallback();
			if (result != RR_SUCCESS)
			{
				return result;
			}
		}

		// Query all available GPU's and store them in the mGPUs vector.
		result = QueryAvailablePhysicalDevices();
		if (result != RR_SUCCESS)
		{
			return result;
		}

		// Pick most suitable physical device.
		FVulkanPhysicalDevice* PhysicalDevice;
		result = PickPhysicalDevice(&PhysicalDevice);
		if (result != RR_SUCCESS)
		{
			return result;
		}

		// Create logical device.
		result = CreateLogicalDevice(PhysicalDevice);
		if (result != RR_SUCCESS)
		{
			return result;
		}

		// The renderer has been properly initialized, therefore we can set the initialized member to true.
		mInitialized = true;

		return RR_SUCCESS;
	}

	bool RVulkanRenderer::IsInitialized() const
	{
		return mInitialized;
	}

	ERenderResult RVulkanRenderer::CreateVulkanInstance()
	{
		// Define vulkan application. Useful for driver optimizations (aka useless for small applications)
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Raven Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Raven";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

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

		// We always require the surface extension to be present, and an surface extension for our platform.
		std::vector<const char*> instanceExtensions;
		instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

		#if defined(PLATFORM_WINDOWS)
			instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#elif defined(PLATFORM_LINUX)
			instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		#endif

		// Enable validation layers if this is a debug build.
		#ifdef NDEBUG
			mShouldEnableValidationLayers = false;
		#else
			mShouldEnableValidationLayers = true;
		#endif

		// Enable debug report extensions if we're running in debug mode.
		if (mShouldEnableValidationLayers)
		{
			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
		
		// Enable validation layers
		if (mShouldEnableValidationLayers && !EnableValidationLayers(createInfo, validationLayers))
		{
			return RR_LAYER_NOT_PRESENT;
		}

		// Enable extensions
		if (!EnableInstanceExtensions(createInfo, instanceExtensions))
		{
			return RR_EXTENSION_NOT_PRESENT;
		}

		// Create the vulkan instance using our vulkan create info structure. We don't define an allocator for now.
		if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
			return RR_INITIALIZATION_FAILED;
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::EnableValidationLayers(VkInstanceCreateInfo& rInfo, const std::vector<const char*>& rInstanceLayers)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		// Retrieve available layers into the availablelayers vector.
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// For each layer that we want enabled, check if we support it.
		for (const char* layerName : rInstanceLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return RR_LAYER_NOT_PRESENT;
			}
		}


		rInfo.enabledLayerCount = static_cast<uint32_t>(rInstanceLayers.size());
		rInfo.ppEnabledLayerNames = rInstanceLayers.data();

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::EnableInstanceExtensions(VkInstanceCreateInfo& rInfo, const std::vector<const char*>& rInstanceExtensions)
	{
		// Query how many extensions are available to us.
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		// Container of extensions.
		std::vector<VkExtensionProperties> extensions(extensionCount);

		// Retrieve the extensions we have available into our extensions vector.
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// For each extension that we want enabled, check if we support it.
		for (const char* extensionName : rInstanceExtensions) {
			bool extFound = false;

			for (const auto& extProperty : extensions) {
				if (strcmp(extensionName, extProperty.extensionName) == 0) {
					extFound = true;
					break;
				}
			}

			if (!extFound) {
				return RR_EXTENSION_NOT_PRESENT;
			}
		}

		// Add the extensions to the create info structure.
		rInfo.enabledExtensionCount = static_cast<uint32_t>(rInstanceExtensions.size());
		rInfo.ppEnabledExtensionNames = rInstanceExtensions.data();

		return RR_SUCCESS;

	}

	VkResult RVulkanRenderer::CreateDebugReportCallbackExtension(VkInstance pInstance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) 
	{
		auto func = GET_VK_FUNC(mInstance, vkCreateDebugReportCallbackEXT);
		if (func != nullptr) {
			return func(mInstance, pCreateInfo, pAllocator, pCallback);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback (VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
	{

		RLOG_WARNING("Vulkan Validation: %s", msg);
		return VK_FALSE;
	}

	ERenderResult RVulkanRenderer::DestroyDebugReportCallbackExtention(VkInstance pInstance, VkDebugReportCallbackEXT pCallback, const VkAllocationCallbacks* pAllocator) {
		auto func = GET_VK_FUNC(mInstance, vkDestroyDebugReportCallbackEXT);
		if (func != nullptr) {
			func(mInstance, pCallback, pAllocator);
			return RR_SUCCESS;
		}

		return RR_EXTENSION_NOT_PRESENT;
	}

	ERenderResult RVulkanRenderer::SetupDebugCallback()
	{
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT; // only errors and warnings.
		createInfo.pfnCallback = DebugCallback;

		// Create debug extension.
		if (CreateDebugReportCallbackExtension(mInstance, &createInfo, nullptr, &mDebuggingCallback) != VK_SUCCESS) {
			return RR_INITIALIZATION_FAILED;
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::QueryAvailablePhysicalDevices()
	{
		// Query how many physical devices we have.
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

		// We don't have any gpu's with vulkan support so we can't proceed.
		if (deviceCount == 0) {
			return RR_NO_SUITABLE_DEVICE;
		}

		// Resize the gpus vector to hold all physical devices.
		mGPUs.resize(deviceCount);

		// Get the list of the physical devices we have.
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

		for (uint32_t i = 0; i < deviceCount; i++)
		{
			mGPUs[i] = FVulkanPhysicalDevice();

			// Fill our physical device structures.
			mGPUs[i].PhysicalDevice = devices[i];

			// Query device properties.
			vkGetPhysicalDeviceProperties(devices[i], &mGPUs[i].Properties);

			// Query device memory properties.
			vkGetPhysicalDeviceMemoryProperties(devices[i], &mGPUs[i].MemoryProperties);

			// Query device features.
			vkGetPhysicalDeviceFeatures(devices[i], &mGPUs[i].Features);

			// Get amount of available queue families.
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);

			// Get the list of queue families.
			mGPUs[i].QueueFamilyProperties.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, mGPUs[i].QueueFamilyProperties.data());

			// query the amount of extensions this physical device supports.
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);

			// Retrieve the extension list.
			mGPUs[i].SupportedExtensions.resize(extensionCount);
			vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, mGPUs[i].SupportedExtensions.data());
		}

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::PickPhysicalDevice(FVulkanPhysicalDevice** ppDevice)
	{
		// Safety check to make sure we have retrieved all information about the physical devices.
		if (mGPUs.size() == 0)
		{
			ERenderResult result = QueryAvailablePhysicalDevices();
			if (result != RR_SUCCESS)
			{
				return result;
			}
		}

		// We require swapchain support for our device.
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// Find the most suitable physical device.
		FVulkanPhysicalDevice* mostSuitableDevice = nullptr;
		uint32_t deviceScore = 0;

		for (auto& candidate : mGPUs)
		{
			// Calculate score for this device.
			uint32_t score = CalculatePhysicalDeviceScore(&candidate, deviceExtensions);

			// If this device is suitable, and better than the current candidate, make this device our best candidate.
			if (score != 0 && score > deviceScore)
			{
				mostSuitableDevice = &candidate;
				deviceScore = score;
			}
		}

		// If we dont have any suitable device, return an error code.
		if (mostSuitableDevice == VK_NULL_HANDLE) {
			return RR_NO_SUITABLE_DEVICE;
		}

		// Output our most suitable device to ppDevice.
		ppDevice = &mostSuitableDevice;

		return RR_SUCCESS;
	}

	FQueueFamilyIndices RVulkanRenderer::RetrieveQueueFamilyIndices(const FVulkanPhysicalDevice* pDevice)
	{
		FQueueFamilyIndices indices;
		indices.GraphicsQueueIndex = RetrieveQueueFamilyIndex(pDevice, VK_QUEUE_GRAPHICS_BIT);
		indices.ComputeQueueIndex = RetrieveQueueFamilyIndex(pDevice, VK_QUEUE_COMPUTE_BIT);
		indices.TransferQueueIndex = RetrieveQueueFamilyIndex(pDevice, VK_QUEUE_TRANSFER_BIT);

		return indices;
	}

	uint32_t RVulkanRenderer::RetrieveQueueFamilyIndex(const FVulkanPhysicalDevice* pDevice, VkQueueFlags flags)
	{
		// Try to find a dedicated compute queue.
		if (flags & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < pDevice->QueueFamilyProperties.size(); i++)
			{
				// If the queue supports compute and does NOT support graphics, we can return this as a dedicated compute queue.
				if (pDevice->QueueFamilyProperties[i].queueCount > 0 && 
					(pDevice->QueueFamilyProperties[i].queueFlags & flags) 
					&& ((pDevice->QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
					return i;
				}
			}
		}

		// Try to find a dedicated transfer queue.
		if (flags & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < pDevice->QueueFamilyProperties.size(); i++)
			{
				// If the queue supports compute and does NOT support graphics or compute, we can return this as a dedicated compute queue.
				if (pDevice->QueueFamilyProperties[i].queueCount > 0 && 
					(pDevice->QueueFamilyProperties[i].queueFlags & flags) && 
					((pDevice->QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && 
					((pDevice->QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) 
				{
					return i;
				}
			}
		}

		// Otherwise return the first queue which matches the flags.
		for (uint32_t i = 0; i < pDevice->QueueFamilyProperties.size(); i++)
		{
			// If the queue supports compute and does NOT support graphics or compute, we can return this as a dedicated compute queue.
			if (pDevice->QueueFamilyProperties[i].queueCount > 0 && 
				(pDevice->QueueFamilyProperties[i].queueFlags & flags)) {
				return i;
			}
		}

		// We couldn't find a queue which supports the specified flags so we return an invalid queue index.
		return UINT32_MAX;
	}

	uint32_t RVulkanRenderer::CalculatePhysicalDeviceScore(const FVulkanPhysicalDevice* pDevice, const std::vector<const char*>& rRequiredDeviceExtensions)
	{
		uint32_t score = 0;

		// We require device extensions to be available.
		if (!HasDeviceExtensionSupport(pDevice, rRequiredDeviceExtensions))
		{
			// We don't have required device extensions therefore this device is unsuitable.
			return 0;
		}

		// Retrieve queue family indices.
		FQueueFamilyIndices queueFamilyIndices = RetrieveQueueFamilyIndices(pDevice);

		// A dedicated GPU is preferred.
		if (pDevice->Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		// If we have dedicated queues for transferral and compute, our score goes up.
		if (queueFamilyIndices.HasDedicatedComputeQueue())
		{
			score += 100;
		}

		if (queueFamilyIndices.HasDedicatedTransferQueue())
		{
			score += 100;
		}

		if (pDevice->Features.tessellationShader)
		{
			score += 100;
		}

		if (pDevice->Features.geometryShader)
		{
			score += 100;
		}

		return score;
	}

	ERenderResult RVulkanRenderer::CreateLogicalDevice(FVulkanPhysicalDevice* pPhysicalDevice)
	{
		// Find queue indices.
		mDevice.QueueIndices = RetrieveQueueFamilyIndices(pPhysicalDevice);

		// Vector of queue creation information structures.
		std::vector<VkDeviceQueueCreateInfo> queueInfos;

		// Create graphics queue.
		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = mDevice.QueueIndices.GraphicsQueueIndex;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
		queueInfos.push_back(graphicsQueueCreateInfo);

		// Create queue creation info for the compute queue if it is dedicated.
		if (mDevice.QueueIndices.HasDedicatedComputeQueue())
		{
			VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
			computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			computeQueueCreateInfo.queueFamilyIndex = mDevice.QueueIndices.ComputeQueueIndex;
			computeQueueCreateInfo.queueCount = 1;
			computeQueueCreateInfo.pQueuePriorities = &queuePriority;
			queueInfos.push_back(computeQueueCreateInfo);
		}

		// Create queue creation info for the transfer queue if it is dedicated.
		if (mDevice.QueueIndices.HasDedicatedTransferQueue())
		{
			VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
			transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			transferQueueCreateInfo.queueFamilyIndex = mDevice.QueueIndices.TransferQueueIndex;
			transferQueueCreateInfo.queueCount = 1;
			transferQueueCreateInfo.pQueuePriorities = &queuePriority;
			queueInfos.push_back(transferQueueCreateInfo);
		}

		// We want certain device features to be enabled.
		mDevice.EnabledFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
		createInfo.pEnabledFeatures = &mDevice.EnabledFeatures;

		createInfo.enabledExtensionCount = 0;

		// Enable the same validation layers for this logical device.
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		// Enable swapchain extensions.
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		if (mShouldEnableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Create the device.
		if (vkCreateDevice(pPhysicalDevice->PhysicalDevice, &createInfo, nullptr, &mDevice.Device) != VK_SUCCESS) {
			return RR_INITIALIZATION_FAILED;
		}

		mDevice.PhysicalDevice = pPhysicalDevice;

		// Retrieve our graphics queue.
		vkGetDeviceQueue(mDevice.Device, mDevice.QueueIndices.GraphicsQueueIndex, 0, &mDevice.GraphicsQueue);
		vkGetDeviceQueue(mDevice.Device, mDevice.QueueIndices.TransferQueueIndex, 0, &mDevice.TransferQueue);
		vkGetDeviceQueue(mDevice.Device, mDevice.QueueIndices.ComputeQueueIndex, 0, &mDevice.ComputeQueue);

		return RR_SUCCESS;
	}

	bool RVulkanRenderer::HasDeviceExtensionSupport(const FVulkanPhysicalDevice* pDevice, const std::vector<const char*>& rDeviceExtensions)
	{
		// Go over the extensions and remove them if we support it. If the list is empty at the end, it means we support all extensions.
		std::set<std::string> requiredExtensions(rDeviceExtensions.begin(), rDeviceExtensions.end());

		for (const auto& extension : pDevice->SupportedExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	ERenderResult RVulkanRenderer::Destroy()
	{
		// If the renderer is not initialized, we have nothing to destroy; thefore we immediately return success.
		if (!IsInitialized())
		{
			return RR_SUCCESS;
		}

		if(mShouldEnableValidationLayers)
		{
			// Destroy debugging report callback.
			DestroyDebugReportCallbackExtention(mInstance, mDebuggingCallback, nullptr);
		}

		// Destroy our logical device.
		vkDestroyDevice(mDevice, nullptr);

		// Destroy our Vulkan instance.
		vkDestroyInstance(mInstance, nullptr);

		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::CreateSwapChain(IPlatformWindow* nativeWindow, ISwapChain** ppSwapChain)
	{
		return RR_SUCCESS;
	}

	ERenderResult RVulkanRenderer::DestroySwapChain(ISwapChain* pSwapchain)
	{
		return RR_SUCCESS;
	}
}
