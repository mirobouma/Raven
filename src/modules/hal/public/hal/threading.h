#pragma once

#ifdef PLATFORM_WINDOWS
#include "platform_windows/threading.h"
#elif PLATFORM_LINUX
#include "platform_linux/threading.h"
#else
#include "platform/generic/threading.h"
#endif
