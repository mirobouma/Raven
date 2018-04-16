#include "PlatformLinux/PlatformLinuxInfo.h"
#include <unistd.h>

namespace Raven {
	
	uint32_t RPlatformLinuxInfo::GetPageSize()
	{
		return (size_t)sysconf(_SC_PAGESIZE);
	}
}
