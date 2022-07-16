#pragma once

#ifdef PLATFORM_WINDOWS
#include "os/windows/windows_thread.h"
#elif PLATFORM_LINUX
#include "os/windows/linux_thread.h"
#endif