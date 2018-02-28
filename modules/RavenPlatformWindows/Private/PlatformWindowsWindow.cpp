#include "PlatformWindows/PlatformWindowsWindow.h"
#include "PlatformWindows/PlatformWindows.h"
#include <locale>

namespace Raven {

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

		RECT wr = { 0, 0, 800, 600 };    // set the size, but not the position
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

		// create the window and use the result as the handle
		HWND winHandle = CreateWindowExW(NULL,
			L"RavenWindowClass1",    // name of the window class
			L"Windows Application <Vulkan>",   // title of the window
			WS_OVERLAPPEDWINDOW,    // window style
			CW_USEDEFAULT,    // x-position of the window
			CW_USEDEFAULT,    // y-position of the window
			wr.right - wr.left,    // width of the window
			wr.bottom - wr.top,    // height of the window
			NULL,    // we have no parent window, NULL
			NULL,    // we aren't using menus, NULL
			GetModuleHandle(nullptr),    // application handle
			nullptr);    // used with multiple windows, NULL

		ShowWindow(winHandle, SW_SHOW);
	}
}


