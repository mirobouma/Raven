#include "PlatformLinux/PlatformLinuxWindow.h"
#include "Platform/WindowSystemInterface.h"

#include "SDL.h"

namespace Raven 
{
	RPlatformLinuxWindow::RPlatformLinuxWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags) : RGenericPlatformWindow(title, size, position, flags)
	{
		// Create the window.
		Create();
	}

	RPlatformLinuxWindow::RPlatformLinuxWindow() : RGenericPlatformWindow()
	{
		// Create the window.
		Create();
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
		Math::FVector2 position = _geometry.Position;
		uint32_t flags = SDL_WINDOW_RESIZABLE;

		// Set up proper variables based on flags.
		if (_flags & EPlatformWindowFlags_AutoPosition)
		{
			position.x = SDL_WINDOWPOS_UNDEFINED;
			position.y = SDL_WINDOWPOS_UNDEFINED;
		}

		if (_flags & EPlatformWindowFlags_Borderless)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (_flags & EPlatformWindowFlags_AlwaysOnTop)
		{
			flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		}

		if (_flags & EPlatformWindowFlags_NoResize)
		{
			flags &= ~SDL_WINDOW_RESIZABLE;
		}

		if (_flags & EPlatformWindowFlags_Shown)
		{
			flags |= SDL_WINDOW_SHOWN;
		}

		// Create SDL window.
		_window = SDL_CreateWindow(
			_title.c_str(),
			position.x,
			position.y,
			_geometry.Size.x,
			_geometry.Size.y,
			flags
		);
	}

	void RPlatformLinuxWindow::HandleWindowMessages()
	{
		if (GWindowSystemInterface == nullptr)
		{
			// Window system interface doesn't exist so we can't handle messages.
			return;
		}

		SDL_Event event;

		while (SDL_PollEvent(&event))
        {
				switch (event.type)
				{
                    case SDL_WINDOWEVENT:
                    {
                        SDL_Window* sdl_win = SDL_GetWindowFromID(event.window.windowID);
                        RGenericPlatformWindow* window = GWindowSystemInterface->GetWindowByNativeHandle(sdl_win);

                            switch (event.window.event)
                            {
                                case SDL_WINDOWEVENT_CLOSE:
                                {
                                    GWindowSystemInterface->OnMessageQuit(window);

                                }
                                return;
                            }
                    }
                    break;
				}
		}
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