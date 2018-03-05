#include "Platform/WindowSystemInterface.h"
#include <algorithm>

namespace Raven {

	/*! Initialize the window system interface to nullptr until properly initialized. */
	RWindowSystemInterface* GWindowSystemInterface = nullptr;

	bool RWindowSystemInterface::Initialize() 
	{
		// Check if we haven't already initialized the window system interface.
		if (GWindowSystemInterface != nullptr)
		{
			// There is already an initialized window system interface, therefore initialization failed.
			return false;
		}

		// Set the global variable to this interface.
		GWindowSystemInterface = new RWindowSystemInterface();
		return true;
	}

	bool RWindowSystemInterface::Destroy() 
	{
		// Check if we have an already initialized window system interface.
		if (GWindowSystemInterface == nullptr)
		{
			// We shouldn't destroy the window system interface because it's not initialized.
			return false;
		}

		// Destroy window system interface.
		delete GWindowSystemInterface;
		GWindowSystemInterface = nullptr;

		return true;
	}


	void RWindowSystemInterface::UpdatePlatformWindows()
	{
		RPlatformWindow::HandleWindowMessages();
	}
	

	bool RWindowSystemInterface::RegisterPlatformWindow(RGenericPlatformWindow* Window)
	{

		// Add window to tracking list.
		_Windows.push_back(Window);

		return true;
	}


	bool RWindowSystemInterface::UnregisterPlatformWindow(RGenericPlatformWindow* Window)
	{
		// Rempve window from tracking list.
		auto it = std::remove(_Windows.begin(), _Windows.end(), Window);

		if (it != _Windows.end())
		{
			return true;
		}

		return false;
	}

	RGenericPlatformWindow* RWindowSystemInterface::GetWindowByNativeHandle(RWindowHandle handle)
	{
		auto foundItem = std::find_if(_Windows.begin(), _Windows.end(), [&handle](RGenericPlatformWindow* wnd)
		{
			return wnd->GetNativeHandle() == handle;
		});

		if (foundItem != _Windows.end())
		{
			return *foundItem;
		}

		return nullptr;
	}

	void RWindowSystemInterface::OnMessageQuit(RGenericPlatformWindow* window)
	{
		// Pass quit message to the window.
		window->OnQuit();
	}
}