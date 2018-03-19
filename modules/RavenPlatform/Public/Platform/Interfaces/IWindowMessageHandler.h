#pragma once
#include <string>

namespace Raven
{
	class IPlatformWindow;

	/*!
	\class IWindowMessageHandler
	\brief The IWindowMessageHandler class is an abstract interface for handling various window system messages.
	*/
	class IWindowMessageHandler
	{
	protected:
		IWindowMessageHandler() = default; /*!< This is an abstract interface so we don't have to instantiate it. */
		IWindowMessageHandler(const IWindowMessageHandler&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a window. */
		IWindowMessageHandler& operator= (const IWindowMessageHandler&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a window. */

	public:
		/*! Handler function when the window is resized. */
		virtual void HandleWindowCreate(IPlatformWindow* window) = 0;

		/*! Handler function when the window is resized. */
		virtual void HandleWindowResize(IPlatformWindow* window, int width, int height) = 0;

		/*! Handler function when the window is moved. */
		virtual void HandleWindowMove(IPlatformWindow* window, int x, int y) = 0;

		/*! Handler function when the window is focussed. */
		virtual void HandleWindowFocus(IPlatformWindow* window) = 0;

		/*! Handler function when the window focus is lost. */
		virtual void HandleWindowFocusLost(IPlatformWindow* window) = 0;

		/*! Handler function when the window posts a close message. */
		virtual void HandleWindowClose(IPlatformWindow* window) = 0;

	};
}

