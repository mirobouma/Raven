#pragma once

#ifdef PLATFORM_WINDOWS
	#include "PlatformWindows/PlatformWindows.h"
#elif PLATFORM_LINUX
	#include "PlatformLinux/PlatformLinux.h"
#else
	#include "Platform/Generic/GenericPlatform.h"
#endif