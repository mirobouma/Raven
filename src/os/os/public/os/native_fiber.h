#pragma once

#ifdef PLATFORM_WINDOWS
#include "os/windows/windows_fiber.h"
#elif PLATFORM_LINUX
#include "os/windows/linux_fiber.h"
#endif