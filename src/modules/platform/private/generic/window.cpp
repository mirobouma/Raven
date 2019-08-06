#include "platform/generic/window.h"
#include "core/math/math.h"
#include "core/math/rect.h"

namespace Raven {
	RGenericPlatformWindow::RGenericPlatformWindow(IWindowMessageHandler* handler) : _handler(handler)
	{
		// Set default properties.
		InternalInitialize(
			"New Window",
			FRect(FVector2(800, 600), FVector2(0, 0)), 
			EPlatformWindowFlags::EPlatformWindowFlags_AutoPosition
		);
	}

	RGenericPlatformWindow::~RGenericPlatformWindow()
	{
		_handler->HandleWindowClose(this);
	}

	RGenericPlatformWindow::RGenericPlatformWindow(const char* title, FVector2 size, FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler) : _handler(handler)
	{
		// Set default properties.
		InternalInitialize(title, FRect(size, position), flags);
	}

	void RGenericPlatformWindow::Create()
	{
		_handler->HandleWindowCreate(this);
	}

	void RGenericPlatformWindow::InternalInitialize(const char* title, FRect geometry, EPlatformWindowFlags flags) {
		_geometry = geometry;
		_flags = flags;
		_title = title;
	}
}