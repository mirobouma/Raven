#include "PlatformWindows/PlatformWindowsWindow.h"
#include "PlatformWindows/PlatformWindows.h"
#include "PlatformWindows/Util/WString.h"

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

	RPlatformWindowsWindow::RPlatformWindowsWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags) 
		: RGenericPlatformWindow(title, size, position, flags), _winHandle(new WindowsWinHandle())
	{
		// Create the window.
		Create();
	}

	RPlatformWindowsWindow::RPlatformWindowsWindow() : RGenericPlatformWindow(), _winHandle(new WindowsWinHandle())
	{
		// Create the window.
		Create();
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
		Math::FVector2 position = _geometry.Position;
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
			nullptr);    // used with multiple windows, NULL

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

	void RPlatformWindowsWindow::Raise()
	{
		BringWindowToTop(_winHandle->handle);
	}

	void RPlatformWindowsWindow::SetTitle(const char* title)
	{
		SetWindowTextW(_winHandle->handle, Util::String::ConvertStringToWString(title).c_str());
		_title = title;
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


