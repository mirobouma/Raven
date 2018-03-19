#ifdef PLATFORM_WINDOWS
	#include "PlatformWindows/PlatformWindowsWindow.h"
#elif PLATFORM_LINUX
	#include "PlatformLinux/PlatformLinuxWindow.h"
#else
	#include "Platform/Generic/GenericPlatformWindow.h"
#endif
