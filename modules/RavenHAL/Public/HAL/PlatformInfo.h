#pragma once

#ifdef PLATFORM_WINDOWS
	#include "PlatformWindows/PlatformWindowsInfo.h"
#elif PLATFORM_LINUX
	#include "PlatformLinux/PlatformLinuxInfo.h"
#else
	#include "Platform/Generic/GenericPlatformInfo.h"
#endif
