#include "platform_windows/platform.h"
#include "platform_windows/window.h"
#include "core/input/input_manager.h"
#include "imgui/imgui.h"

namespace Raven {

	uint32_t gCurrWidth = 0;
	uint32_t gCurrHeight = 0;

	// this is the main message handler for the program
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			wnd->GetMessageHandler()->HandleWindowClose(wnd);
		return 0;
		}
		case WM_SIZE:
		{
			gCurrWidth = LOWORD(lParam);
			gCurrHeight = HIWORD(lParam);

			//GInputManager->SetDisplaySize(gCurrWidth, gCurrHeight);
		}
		case WM_EXITSIZEMOVE:
		{
			if (gCurrWidth != 0 && gCurrHeight != 0)
			{
				wnd->GetMessageHandler()->HandleWindowResize(wnd, gCurrWidth, gCurrHeight);
			}
		}
		return 0;
		
		}

		if (ImGui::GetCurrentContext() != nullptr)
		{
			ImGuiIO& io = ImGui::GetIO();

			switch (message)
			{
			case WM_LBUTTONDOWN:
				io.MouseDown[0] = true;
				return true;
			case WM_LBUTTONUP:
				io.MouseDown[0] = false;
				return true;
			case WM_RBUTTONDOWN:
				io.MouseDown[1] = true;
				return true;
			case WM_RBUTTONUP:
				io.MouseDown[1] = false;
				return true;
			case WM_MOUSEWHEEL:
				io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
				return true;
			case WM_MOUSEMOVE:
				io.MousePos.x = (signed short)(lParam);
				io.MousePos.y = (signed short)(lParam >> 16);
				return true;
			case WM_KEYDOWN:
				if (wParam < 512)
				{
					io.KeysDown[wParam] = 1;
				}

				io.KeyCtrl = io.KeysDown[VK_CONTROL];
				io.KeyShift = io.KeysDown[VK_SHIFT];
				io.KeyAlt = io.KeysDown[VK_MENU];
				io.KeySuper = io.KeysDown[VK_LWIN] | io.KeysDown[VK_RWIN];

				return true;
			case WM_KEYUP:
				if (wParam < 512)
				{
					io.KeysDown[wParam] = 0;
				}

				io.KeyCtrl = io.KeysDown[VK_CONTROL];
				io.KeyShift = io.KeysDown[VK_SHIFT];
				io.KeyAlt = io.KeysDown[VK_MENU];
				io.KeySuper = io.KeysDown[VK_LWIN] | io.KeysDown[VK_RWIN];

				return true;
			case WM_CHAR:
				// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
				if (wParam > 0 && wParam < 0x10000)
					io.AddInputCharacter((unsigned short)wParam);
				return true;
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