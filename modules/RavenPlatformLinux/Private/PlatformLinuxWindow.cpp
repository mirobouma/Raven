#include "PlatformLinux/PlatformLinuxWindow.h"
#include "SDL.h"
#include "imgui/imgui.h"

namespace Raven 
{
    uint32_t mouse_x = 0;
    uint32_t mouse_y = 0;

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
							case SDL_WINDOWEVENT_RESIZED:
							{
								if (event.window.data1 != 0 && event.window.data2 != 0)
								{
									window->_handler->HandleWindowResize(window, event.window.data1, event.window.data2);
								}
							}break;
							}
						}
                    }
                    break;
				}

			if (ImGui::GetCurrentContext() != nullptr) {
                ImGuiIO &io = ImGui::GetIO();

                switch (event.type) {
                    case SDL_MOUSEBUTTONDOWN: {
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            io.MouseDown[0] = true;
                        }
                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            io.MouseDown[1] = true;
                        }
                    }
                        break;
                    case SDL_MOUSEBUTTONUP: {
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            io.MouseDown[0] = false;
                        }
                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            io.MouseDown[1] = false;
                        }
                    }
                    break;
                    case SDL_MOUSEWHEEL: {
                        if (event.wheel.x > 0) io.MouseWheelH += 1;
                        if (event.wheel.x < 0) io.MouseWheelH -= 1;
                        if (event.wheel.y > 0) io.MouseWheel += 1;
                        if (event.wheel.y < 0) io.MouseWheel -= 1;
                    }
                    break;
                    case SDL_MOUSEMOTION: {
                        io.MousePos.x = event.motion.x;
                        io.MousePos.y = event.motion.y;
                    }
                    break;
                    case SDL_KEYDOWN:
                    {
                        if (event.key.keysym.scancode < 512) {
                            io.KeysDown[event.key.keysym.scancode] = 1;
                        }

                        io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                        io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                        io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
                        io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

                    }break;
                    case SDL_KEYUP: {
                        if (event.key.keysym.scancode < 512) {
                            io.KeysDown[event.key.keysym.scancode] = 0;
                        }

                        io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                        io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                        io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
                        io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

                    }break;
                    case SDL_TEXTINPUT:
                    {
                        io.AddInputCharactersUTF8(event.text.text);
                    }
                    break;

                }
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