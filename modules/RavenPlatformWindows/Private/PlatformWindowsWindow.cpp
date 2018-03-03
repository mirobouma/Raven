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

		if (_flags & EPlatformWindowFlags::EPlatformWindowFlags_Shown)
		{
			// Automatically show the window.
			Show();
		}
	}

	RPlatformWindowsWindow::RPlatformWindowsWindow() : RGenericPlatformWindow(), _winHandle(new WindowsWinHandle())
	{
		// Create the window.
		Create();

		if (_flags & EPlatformWindowFlags::EPlatformWindowFlags_Shown)
		{
			// Automatically show the window.
			Show();
		}
	} 

	// this is the main message handler for the program
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		// Process window messages.
		switch (message)
		{
			// this message is read when the window is closed
			case WM_DESTROY:
				// close the application entirely
				PostQuitMessage(0);
			break;
		}

		// Handle any messages the switch statement didn't
		return DefWindowProcW(hWnd, message, wParam, lParam);
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
		WNDCLASSEXW winClass;

		// clear out the window class for use
		ZeroMemory(&winClass, sizeof(WNDCLASSEXW));


		// fill in the struct with the needed information
		winClass.cbSize = sizeof(WNDCLASSEXW);
		winClass.style = CS_HREDRAW | CS_VREDRAW;
		winClass.lpfnWndProc = WindowProc;
		winClass.hInstance = GetModuleHandle(nullptr);
		winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		winClass.lpszClassName = L"RavenWindowClass1";

		// register the window class
		RegisterClassExW(&winClass);

		RECT wr = { 0, 0, _geometry.Size.x, _geometry.Size.y }; // set the size, but not the position
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size to account for the window title bar.

		// create the window and use the result as the handle
		_winHandle->handle = CreateWindowExW(NULL,
			L"RavenWindowClass1",    // name of the window class
			Util::String::ConvertStringToWString(_title).c_str(), // title of the window
			WS_OVERLAPPEDWINDOW,    // window style
			CW_USEDEFAULT,    // x-position of the window
			CW_USEDEFAULT,    // y-position of the window
			wr.right - wr.left,    // width of the window
			wr.bottom - wr.top,    // height of the window
			NULL,    // we have no parent window, NULL
			NULL,    // we aren't using menus, NULL
			GetModuleHandle(nullptr),    // application handle
			nullptr);    // used with multiple windows, NULL
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


