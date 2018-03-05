#include "Platform/GenericPlatformWindow.h"
#include "Platform/WindowSystemInterface.h"
#include "Math/Math.h"
#include "Math/Rect.h"

namespace Raven {
	RGenericPlatformWindow::RGenericPlatformWindow()
	{
		// Set default properties.
		InternalInitialize(
			"New Window",
			Math::FRect(Math::FVector2(800, 600), Math::FVector2(0, 0)), 
			EPlatformWindowFlags::EPlatformWindowFlags_AutoPosition
		);
	}

	RGenericPlatformWindow::~RGenericPlatformWindow()
	{
		if (GWindowSystemInterface != nullptr)
		{
			GWindowSystemInterface->UnregisterPlatformWindow(this);
		}
	}

	RGenericPlatformWindow::RGenericPlatformWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags)
	{
		// Set default properties.
		InternalInitialize(title, Math::FRect(size, position), flags);
	}

	void RGenericPlatformWindow::InternalInitialize(const char* title, Math::FRect geometry, EPlatformWindowFlags flags) {
		_geometry = geometry;
		_flags = flags;
		_title = title;
		_shouldQuit = false;

		if (GWindowSystemInterface != nullptr)
		{
			// Register this window to the windowing system.
			GWindowSystemInterface->RegisterPlatformWindow(this);
		}
	}
}