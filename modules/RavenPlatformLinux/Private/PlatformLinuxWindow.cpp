#include "PlatformLinux/PlatformLinuxWindow.h"
#include "SDL.h"

namespace Raven 
{

	RPlatformLinuxWindow::RPlatformLinuxWindow(const char* title, FVector2 size, FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler) : _handler(handler)
	{
		// Set default properties.
		InternalInitialize(title, FRect(size, position), flags);
	}

	RPlatformLinuxWindow::RPlatformLinuxWindow(IWindowMessageHandler* handler) : _handler(handler)
	{
		// Set default properties.
		InternalInitialize(
			"New Window <Linux>",
			FRect(FVector2(800, 600), FVector2(0, 0)),
			EPlatformWindowFlags::EPlatformWindowFlags_AutoPosition
		);
	}

	void RPlatformLinuxWindow::InternalInitialize(const char* title, FRect geometry, EPlatformWindowFlags flags) {
		_geometry = geometry;
		_flags = flags;
		_title = title;
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
		FVector2 position = _geometry.Position;
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

        if(_window != nullptr) {

            // Set pointer to this window instance.
            SDL_SetWindowData(_window, "wnd", this);

            // Handle window creation.
            _handler->HandleWindowCreate(this);
        }
	}

	void RPlatformLinuxWindow::HandleWindowMessages()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
        {
				switch (event.type)
				{
                    case SDL_WINDOWEVENT:
                    {
                        SDL_Window* sdl_win = SDL_GetWindowFromID(event.window.windowID);
						RPlatformLinuxWindow* window = (RPlatformLinuxWindow*)SDL_GetWindowData(sdl_win, "wnd");

						if (window != nullptr)
						{
							switch (event.window.event)
							{
							case SDL_WINDOWEVENT_CLOSE:
							{
								window->_handler->HandleWindowClose(window);

							}
							return;
							}
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
	
	void RPlatformLinuxWindow::OnCreate()
	{
		// Unimplemented.
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