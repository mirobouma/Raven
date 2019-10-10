#pragma once
#if defined(WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.h>
#include "core/logging_macros.h"

#undef max
#undef min
#include "vma/vk_mem_alloc.h"

/*! Helper macro to dynamically find the address of a specific vulkan (extension) function. */
#define GET_VK_FUNC(I, T) (PFN_ ## T)vkGetInstanceProcAddr(I, #T)

#define MAX_RESOURCE_BARRIERS 16