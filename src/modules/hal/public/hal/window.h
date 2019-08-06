#pragma once

#ifdef PLATFORM_WINDOWS
	#include "platform_windows/window.h"
#elif PLATFORM_LINUX
	#include "platform_linux/window.h"
#else
	#include "platform/generic/window.h"
#endif
