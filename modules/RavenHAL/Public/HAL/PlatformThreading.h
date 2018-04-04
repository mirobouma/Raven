#pragma once

#ifdef PLATFORM_WINDOWS
#include "PlatformWindows/PlatformWindowsThreading.h"
#elif PLATFORM_LINUX
#include "PlatformLinux/PlatformLinuxThreading.h"
#else
#include "Platform/Generic/GenericPlatformThreading.h"
#endif
