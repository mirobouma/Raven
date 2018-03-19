#include "PlatformWindows/PlatformWindowsInfo.h"
#include "PlatformWindows/PlatformWindows.h"

namespace Raven 
{
	uint32_t RPlatformWindowsInfo::GetPageSize()
	{
		// Get system information and fetch the page size.
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		return info.dwPageSize;
	}
}
