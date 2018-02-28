#pragma once

#include "Platform/GenericPlatform.h"

#ifdef PLATFORM_WINDOWS
	#include "PlatformWindows/PlatformWindows.h"
#endif
#ifdef PLATFORM_LINUX
	#include "PlatformLinux/PlatformLinux.h"
#endif