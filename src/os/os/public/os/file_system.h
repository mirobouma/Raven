#pragma once

#ifdef PLATFORM_WINDOWS
#include "os/windows/windows_file_system.h"
#elif PLATFORM_LINUX
#include "os/linux/linux_file_system.h"
#endif