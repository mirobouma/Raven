#include "Platform/Generic/GenericPlatformWindow.h"
#include "Math/Math.h"
#include "Math/Rect.h"

namespace Raven {
	RGenericPlatformWindow::RGenericPlatformWindow(IWindowMessageHandler* handler) : _handler(handler)
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
		_handler->HandleWindowClose(this);
	}

	RGenericPlatformWindow::RGenericPlatformWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler) : _handler(handler)
	{
		// Set default properties.
		InternalInitialize(title, Math::FRect(size, position), flags);
	}

	void RGenericPlatformWindow::Create()
	{
		_handler->HandleWindowCreate(this);
	}

	void RGenericPlatformWindow::InternalInitialize(const char* title, Math::FRect geometry, EPlatformWindowFlags flags) {
		_geometry = geometry;
		_flags = flags;
		_title = title;
		
		Create();
	}
}