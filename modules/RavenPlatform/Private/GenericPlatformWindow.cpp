#include "Platform/GenericPlatformWindow.h"
#include "Math/Math.h"
#include "Math/Rect.h"

namespace Raven {
	RGenericPlatformWindow::RGenericPlatformWindow()
	{
		// Set default properties.
		_geometry = Math::FRect(Math::FVector2(800, 600), Math::FVector2(0, 0));
		_flags = EPlatformWindowFlags::EPlatformWindowFlags_AutoPosition;
		_title = "Window";

	}

	RGenericPlatformWindow::RGenericPlatformWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags)
	{
		// Set properties.
		_geometry = Math::FRect(size, position);
		_flags = flags;
		_title = title;

	}
}