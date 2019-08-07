#include "platform_windows/window.h"
#include "platform_windows/platform.h"
#include "platform_windows/util/wstring.h"
#include "core/input/input_manager.h"

#include <locale>

namespace Raven {

	// Pimpl idiom.
	struct WindowsWinHandle {
		HWND handle;
	};

	void WindowsWinHandleDeleter::operator()(WindowsWinHandle *p)
	{
		delete p;
	}

	RPlatformWindowsWindow::RPlatformWindowsWindow(const char* title, FVector2 size, FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler)
		: _winHandle(new WindowsWinHandle()), _handler(handler)
	{
		// Set default properties.
		InternalInitialize(title, FRect(size, position), flags);
	}

	RPlatformWindowsWindow::RPlatformWindowsWindow(IWindowMessageHandler* handler) : _winHandle(new WindowsWinHandle()), _handler(handler)
	{
		// Set default properties.
		InternalInitialize(
			"New Window <Windows>",
			FRect(FVector2(800, 600), FVector2(0, 0)),
			EPlatformWindowFlags::EPlatformWindowFlags_AutoPosition
		);
	} 

	void RPlatformWindowsWindow::InternalInitialize(const char* title, FRect geometry, EPlatformWindowFlags flags) 
	{
		_geometry = geometry;
		_flags = flags;
		_title = title;
	}

	void RPlatformWindowsWindow::Show()
	{
		ShowWindow(_winHandle->handle, SW_SHOW);
	}

	void RPlatformWindowsWindow::Hide()
	{
		ShowWindow(_winHandle->handle, SW_HIDE);
	}

	RWindowHandle RPlatformWindowsWindow::GetNativeHandle()
	{
		return _winHandle->handle;
	}

	void RPlatformWindowsWindow::Create()
	{
		FVector2 position = _geometry.Position;
		long flags;

		// Set up proper variables based on flags.
		if (_flags & EPlatformWindowFlags_AutoPosition)
		{
			position.x = static_cast<float>(CW_USEDEFAULT);
			position.y = static_cast<float>(CW_USEDEFAULT);
		}

		if (_flags & EPlatformWindowFlags_Borderless)
		{
			flags = WS_POPUP;
		}
		else
		{
			flags = WS_OVERLAPPEDWINDOW;
		}

		if (_flags & EPlatformWindowFlags_AlwaysOnTop)
		{
			flags |= WS_EX_TOPMOST;
		}

		if (_flags & EPlatformWindowFlags_NoResize)
		{
			flags &= ~WS_THICKFRAME;
			flags &= ~WS_MAXIMIZEBOX;
		}

		RECT wr = { 0, 0, static_cast<long>(_geometry.Size.x), static_cast<long>(_geometry.Size.y) }; // set the size, but not the position
		AdjustWindowRect(&wr, flags, FALSE); // adjust the size to account for the window title bar.

		// create the window and use the result as the handle
		_winHandle->handle = CreateWindowExW(NULL,
			L"RavenWindowClass1",    // name of the window class
			Util::String::ConvertStringToWString(_title).c_str(), // title of the window
			flags,    // window style
			static_cast<int>(position.x),    // x-position of the window
			static_cast<int>(position.y),    // y-position of the window
			wr.right - wr.left,    // width of the window
			wr.bottom - wr.top,    // height of the window
			NULL,    // we have no parent window, NULL
			NULL,    // we aren't using menus, NULL
			GetModuleHandle(nullptr),    // application handle
			this); 

		if (_winHandle->handle != nullptr)
		{
			_handler->HandleWindowCreate(this);
		}

		// Show the window if it should be shown by default.
		if (_flags & EPlatformWindowFlags_Shown)
		{
			Show();
		}

		if (_flags & EPlatformWindowFlags_AlwaysOnTop)
		{
			SetWindowPos(_winHandle->handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	void RPlatformWindowsWindow::HandleWindowMessages()
	{
		// this struct holds Windows event messages
		MSG msg;

		//GInputManager->GetInputManager()->Update();

		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);

			//GInputManager->GetInputManager()->HandleMessage(msg);
		}
	}

	void RPlatformWindowsWindow::Raise()
	{
		BringWindowToTop(_winHandle->handle);
	}

	void RPlatformWindowsWindow::SetTitle(const char* title)
	{
		SetWindowTextW(_winHandle->handle, Util::String::ConvertStringToWString(title).c_str());
		_title = title;
	}

	void RPlatformWindowsWindow::OnCreate()
	{
		// Unimplemented.
	}

	void RPlatformWindowsWindow::OnResize(int width, int height)
	{
		// Unimplemented.
	}


	void RPlatformWindowsWindow::OnMove(int x, int y)
	{
		// Unimplemented.
	}


	void RPlatformWindowsWindow::OnFocus()
	{
		// Unimplemented.
	}

	void RPlatformWindowsWindow::OnFocusLost()
	{
		// Unimplemented.
	}
}


