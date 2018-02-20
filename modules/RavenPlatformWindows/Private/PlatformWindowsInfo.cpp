#include "PlatformWin32\PlatformWindowsInfo.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Raven {
	bool RPlatformWin32::Initialize()
	{
		return IGenericPlatform::Initialize();
	}

	bool RPlatformWin32::Destroy()
	{
		return true;
	}

	uint32_t RPlatformWin32::GetPageSize()
	{
		// Get system information and fetch the page size.
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		return info.dwPageSize;
	}
}
