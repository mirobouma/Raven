#include "Platform/GenericPlatformApplication.h"
#include "Platform/WindowSystemInterface.h"

namespace Raven {
	void RGenericPlatformApplication::Update()
	{
		if (GWindowSystemInterface == nullptr)
		{
			return;
		}

		GWindowSystemInterface->UpdatePlatformWindows();
	}
}