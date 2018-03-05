#pragma once
#include "Platform/GenericPlatformWindow.h"

struct SDL_Window;

namespace Raven {
	/*!
	\class RPlatformLinuxWindow
	\brief Linux platform window.
	*/
	class RPlatformLinuxWindow: public RGenericPlatformWindow {
	public:
		/*! Initialize the window with the specified position, location and flags.
		\param size size of the window.
		\param position position of the window, to automatically choose a position add the EPlatformWindowFlags_AutoPosition flag.
		\param flags Flags for window creation.
		\sa EPlatformWindowFlags
		*/
		RPlatformLinuxWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags);
		RPlatformLinuxWindow(); /*!< Initialize window with default location and size. */
		
		/*! Show the window. */
		virtual void Show() override;

		/*! Hide the window. */
		virtual void Hide() override;

		/*! Raise the window to the top. */
		virtual void Raise() override;

		/*! Set window title. */
		virtual void SetTitle(const char* title);

		/*! Get native window handle. */
		virtual RWindowHandle GetNativeHandle() override;

	protected:
		/*! Friend window system interface so it can call the HandlewindowMessages method. */
		friend class RWindowSystemInterface;

		/*! Handle window messages from the platform. */
		static void HandleWindowMessages();

		/*! Create window (internal) */
		virtual void Create() override;

		/*! Callback function when the window is resized. */
		virtual void OnResize(int width, int height) override;

		/*! Callback function when the window is moved. */
		virtual void OnMove(int x, int y) override;

		/*! Callback function when the window is focussed. */
		virtual void OnFocus() override;

		/*! Callback function when the window focus is lost. */
		virtual void OnFocusLost() override;

		/*! Internal handle to the window. */
		SDL_Window* _window;
	};

	typedef RPlatformLinuxWindow RPlatformWindow;
}
