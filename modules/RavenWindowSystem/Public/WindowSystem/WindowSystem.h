#pragma once
#include "Platform/Interfaces/IPlatformWindow.h"
#include "Platform/Interfaces/IWindowMessageHandler.h"

#include <vector>

namespace Raven {
	/*!
	\class RWindowSystem
	\brief Helper object to communicate platform-specific events such as resizing 
	and window creation to their respective abstracted windows.
	*/
	class RWindowSystem : public IWindowMessageHandler {
	protected:
		RWindowSystem() = default;
		RWindowSystem(const RWindowSystem&) = delete; /*!< Delete copy constuctor */
		RWindowSystem& operator= (const RWindowSystem&) = delete; /*!< Delete assignment constuctor */

	public:
		/*! Initialize the window system interface. */
		static bool Initialize();

		/*! Destroy the window system interface. */
		static bool Destroy();

		/*! Templated function to create a user-specified window of a certain type. Called CreateAWindow instead of CreateWindow because windows likes to define these functions for us q.q */
		template<typename T, typename... Args>
		T* CreateAWindow(Args&&... args)
		{
			// C++17 only, commented out since we only officially support up to c++14.
			//if constexpr (std::is_same_v<T, IPlatformWindow>) 
			//{

			// Instantiate the window.
			T* window = new T(std::forward<Args>(args)..., this);
			return window;

			//}

			return nullptr;
		}

		/*! Retrieves a platform window by handle. */
		IPlatformWindow* GetWindowByNativeHandle(RWindowHandle handle);

		/*! Calls the proper window message handlers to process window messages. */
		void ProcessWindowMessages();

		/*! Returns the amount of alive windows. */
		uint32_t GetNumAliveWindows() const { return _Windows.size(); }

	protected:

		/*! The engine initializes the window system interface, therefore we need to friend the class. */
		friend class REngine;
		friend class IPlatformApplication;

		/*! List of currently created windows. */
		std::vector<IPlatformWindow*> _Windows;

		// Inherited via IWindowMessageHandler
		virtual void HandleWindowCreate(IPlatformWindow* window) override;
		virtual void HandleWindowResize(IPlatformWindow* window, int width, int height) override;
		virtual void HandleWindowMove(IPlatformWindow* window, int x, int y) override;
		virtual void HandleWindowFocus(IPlatformWindow* window) override;
		virtual void HandleWindowFocusLost(IPlatformWindow* window) override;
		virtual void HandleWindowClose(IPlatformWindow* window) override;
	};

	/*! External reference to Global window system object. */
	extern RWindowSystem* GWindowSystem;
}
