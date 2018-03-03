#include "PlatformLinux/PlatformLinuxWindow.h"
#include "SDL.h"

namespace Raven 
{
	RPlatformLinuxWindow::RPlatformLinuxWindow(const char * title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags) : RGenericPlatformWindow(title, size, position, flags)
	{
		// Create the window.
		Create();

		if (_flags & EPlatformWindowFlags::EPlatformWindowFlags_Shown)
		{
			// Automatically show the window.
			Show();
		}
	}

	RPlatformLinuxWindow::RPlatformLinuxWindow() : RGenericPlatformWindow()
	{
		// Create the window.
		Create();

		if (_flags & EPlatformWindowFlags::EPlatformWindowFlags_Shown)
		{
			// Automatically show the window.
			Show();
		}
	}

	void RPlatformLinuxWindow::Show()
	{
		SDL_ShowWindow(_window);
	}

	void RPlatformLinuxWindow::Hide()
	{
		SDL_HideWindow(_window);
	}

	RWindowHandle RPlatformLinuxWindow::GetNativeHandle()
	{
		return _window;
	}

	void RPlatformLinuxWindow::Create()
	{
		// Create SDL window.
		_window = SDL_CreateWindow(
			_title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			_geometry.Size.x,
			_geometry.Size.y,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
		);
	}

	void RPlatformLinuxWindow::Raise()
	{
		SDL_RaiseWindow(_window);
	}

	void RPlatformLinuxWindow::SetTitle(const char* title)
	{
		_title = title;
		SDL_SetWindowTitle(_window, title);
	}
	
	void RPlatformLinuxWindow::OnResize(int width, int height)
	{
		// Unimplemented.
	}


	void RPlatformLinuxWindow::OnMove(int x, int y)
	{
		// Unimplemented.
	}


	void RPlatformLinuxWindow::OnFocus()
	{
		// Unimplemented.
	}

	void RPlatformLinuxWindow::OnFocusLost()
	{
		// Unimplemented.
	}

}