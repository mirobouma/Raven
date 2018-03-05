#include "PlatformWindows/PlatformWindows.h"
#include "PlatformWindows/PlatformWindowsWindow.h"
#include "Platform/WindowSystemInterface.h"

namespace Raven {

	// this is the main message handler for the program
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (GWindowSystemInterface != nullptr)
		{
			RPlatformWindowsWindow* wnd = reinterpret_cast<RPlatformWindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			// Process window messages.
			switch (message)
			{
			case WM_CREATE:
			{
				LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
				SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
			}
			return 0;

			case WM_DESTROY:
			{
				GWindowSystemInterface->OnMessageQuit(wnd);
			}
			return 0;
		
			}
		}

		// Handle any messages the switch statement didn't
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	bool RPlatformWindows::Initialize()
	{
		// Register window type.
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

		// Initialization succeded, return true.
		return true;
	}

	bool RPlatformWindows::Destroy()
	{
		// Deinitialization succeded, return true.
		return true;
	}

}