#pragma once

#ifdef PLATFORM_WINDOWS
#include "os/windows/windows_window.h"
#elif PLATFORM_LINUX
#include "os/linux/linux_window.h"
#endif