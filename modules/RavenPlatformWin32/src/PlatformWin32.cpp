#include "PlatformWin32\PlatformWin32.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Raven {
	bool RPlatformWin32::Initialize()
	{
		return IPlatform::Initialize();
	}

	bool RPlatformWin32::Destroy()
	{
		return true;
	}

	IPlatformWindow* RPlatformWin32::CreatePlatformWindow(uint32_t width, uint32_t height, const char* title, EPlatformWindowFlags flags)
	{
		return nullptr;
	}

	uint32_t RPlatformWin32::GetPageSize()
	{
		// Get system information and fetch the page size.
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		return info.dwPageSize;
	}
}
