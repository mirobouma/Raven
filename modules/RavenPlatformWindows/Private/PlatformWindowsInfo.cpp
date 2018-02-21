#include "PlatformWindows/PlatformWindowsInfo.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Raven {

	uint32_t RPlatformWindowsInfo::GetPageSize()
	{
		// Get system information and fetch the page size.
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		return info.dwPageSize;
	}
}
