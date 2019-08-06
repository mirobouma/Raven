#pragma once

#ifdef PLATFORM_WINDOWS
#include "platform_windows/logger.h"
#elif PLATFORM_LINUX
#include "platform_linux/logger.h"
#else
#include "platform/generic/logger.h"
#endif
