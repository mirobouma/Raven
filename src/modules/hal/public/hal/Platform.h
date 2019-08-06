#pragma once

#ifdef PLATFORM_WINDOWS
	#include "platform_windows/platform.h"
#elif PLATFORM_LINUX
	#include "platform_linux/platform.h"
#else
	#include "platform/generic/platform.h"
#endif