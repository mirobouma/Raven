#pragma once

#ifdef PLATFORM_WINDOWS
	#include "platform_windows/info.h"
#elif PLATFORM_LINUX
	#include "platform_linux/info.h"
#else
	#include "platform/generic/info.h"
#endif
