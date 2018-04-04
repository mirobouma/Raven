#pragma once

#ifdef PLATFORM_WINDOWS
#include "PlatformWindows/PlatformWindowsLogger.h"
#elif PLATFORM_LINUX
#include "PlatformLinux/PlatformLinuxLogger.h"
#else
#include "Platform/Generic/GenericPlatformLogger.h"
#endif
