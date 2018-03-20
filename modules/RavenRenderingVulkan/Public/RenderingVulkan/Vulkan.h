#pragma once
#if defined(PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(PLATFORM_LINUX)
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.h>

/*! Helper macro to dynamically find the address of a specific vulkan (extension) function. */
#define GET_VK_FUNC(I, T) (PFN_ ## T)vkGetInstanceProcAddr(I, #T)
