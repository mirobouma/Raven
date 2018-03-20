#include "WindowSystem/WindowSystem.h"
#include "HAL/PlatformWindow.h"
#include <algorithm>

namespace Raven 
{

	/*! Initialize the window system interface to nullptr until properly initialized. */
	RWindowSystem* GWindowSystem = nullptr;

	bool RWindowSystem::Initialize()
	{
		// Check if we haven't already initialized the window system interface.
		if (GWindowSystem != nullptr)
		{
			// There is already an initialized window system interface, therefore initialization failed.
			return false;
		}

		// Set the global variable to this interface.
		GWindowSystem = new RWindowSystem();
		return true;
	}

	bool RWindowSystem::Destroy()
	{
		// Check if we have an already initialized window system interface.
		if (GWindowSystem == nullptr)
		{
			// We shouldn't destroy the window system interface because it's not initialized.
			return false;
		}

		// Destroy window system interface.
		delete GWindowSystem;
		GWindowSystem = nullptr;

		return true;
	}


	void RWindowSystem::ProcessWindowMessages()
	{
		RPlatformWindow::HandleWindowMessages();
	}

	void RWindowSystem::HandleWindowCreate(IPlatformWindow* window)
	{
		// Add window to tracking list.
		_Windows.push_back(window);

		window->OnCreate();
	}

	void RWindowSystem::HandleWindowResize(IPlatformWindow* window, int width, int height)
	{
		window->OnResize(width, height);
	}

	void RWindowSystem::HandleWindowMove(IPlatformWindow* window, int x, int y)
	{
		window->OnMove(x, y);
	}

	void RWindowSystem::HandleWindowFocus(IPlatformWindow* window)
	{
		window->OnFocus();
	}

	void RWindowSystem::HandleWindowFocusLost(IPlatformWindow* window)
	{
		window->OnFocusLost();
	}

	void RWindowSystem::HandleWindowClose(IPlatformWindow* window)
	{
		window->OnClose();

		// Rempve window from tracking list.
		auto it = std::remove(_Windows.begin(), _Windows.end(), window);

		if (it != _Windows.end())
		{
			_Windows.erase(it);
		}
	}
	

	IPlatformWindow* RWindowSystem::GetWindowByNativeHandle(RWindowHandle handle)
	{
		auto foundItem = std::find_if(_Windows.begin(), _Windows.end(), [&handle](IPlatformWindow* wnd)
		{
			return wnd->GetNativeHandle() == handle;
		});

		if (foundItem != _Windows.end())
		{
			return *foundItem;
		}

		return nullptr;
	}
}