#include "PlatformWindows/PlatformWindows.h"

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