#pragma once
#include "Core/Service/Service.h"
#include "Platform/GenericPlatformWindow.h"
#include "HAL/PlatformWindow.h"

#include <vector>

namespace Raven {
	/*!
	\class RWindowSystemInterface
	\brief Helper object to communicate platform-specific events such as resizing 
	and window creation to their respective abstracted windows.
	*/
	class RWindowSystemInterface {
	protected:
		RWindowSystemInterface() = default;
		RWindowSystemInterface(const RWindowSystemInterface&) = delete; /*!< Delete copy constuctor */
		RWindowSystemInterface& operator= (const RWindowSystemInterface&) = delete; /*!< Delete assignment constuctor */

	public:
		/*! Initialize the window system interface. */
		static bool Initialize();

		/*! Destroy the window system interface. */
		static bool Destroy();

		/*! Register Platform Window to window system interface. */
		bool RegisterPlatformWindow(RGenericPlatformWindow* Window);

		/*! Register Platform Window to window system interface. */
		bool UnregisterPlatformWindow(RGenericPlatformWindow* Window);

		/*! Retrieves a platform window by handle. */
		RGenericPlatformWindow* GetWindowByNativeHandle(RWindowHandle handle);

		/*! Handle quit message of window. */
		void OnMessageQuit(RGenericPlatformWindow* wnd);

	protected:
		/*! The engine initializes the window system interface, therefore we need to friend the class. */
		friend class REngine;
		friend class RGenericPlatformApplication;
		friend class RGenericPlatformWindow;

		/*! Calls the proper window message handlers to process window messages. */
		void UpdatePlatformWindows();

		/*! List of currently created windows. */
		std::vector<RGenericPlatformWindow*> _Windows;
	};

	/*! External reference to Global window system interface object. */
	extern RWindowSystemInterface* GWindowSystemInterface;
}
