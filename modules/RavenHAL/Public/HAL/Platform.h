#pragma once

#if defined(_MSC_VER)
	#if defined(__cplusplus)
		#ifdef RAVEN_EXPORTS 
			#define RVN_EXPORT extern "C" __declspec(dllexport)
		#else
			#define RVN_EXPORT extern "C" __declspec(dllimport)
		#endif
	#else
		#ifdef RAVEN_EXPORTS 
			#define RVN_EXPORT __declspec(dllexport)
		#else
			#define RVN_EXPORT __declspec(dllimport)
		#endif
	#endif
#endif // defined(_MSC_VER)

#if defined(__GNUC__) // clang & gcc
	#if defined(__cplusplus)
		#define RVN_EXPORT extern "C" __attribute__((visibility("default")))
	#else
		#define RVN_EXPORT __attribute__((visibility("default")))
	#endif
#endif // defined(__GNUC__)


#ifdef PLATFORM_WINDOWS
	#include "PlatformWindows/PlatformWindows.h"
#elif PLATFORM_LINUX
	#include "PlatformLinux/PlatformLinux.h"
#else
	#include "Platform/Generic/GenericPlatform.h"
#endif