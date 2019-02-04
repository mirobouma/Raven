#pragma once
#include "Core/Math/Math.h"
#include "Core/Math/Rect.h"
#include "Platform/Defines.h"
#include "Platform/Interfaces/IWindowMessageHandler.h"
#include <string>

namespace Raven 
{
	/*!
	\class IPlatformWindow
	\brief Abstract interface of a platform window.
	*/
	class IPlatformWindow 
	{
	protected:
		IPlatformWindow() = default; /*!< This is an abstract interface so we don't have to instantiate it. */
		IPlatformWindow(const IPlatformWindow&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a window. */
		IPlatformWindow& operator= (const IPlatformWindow&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a window. */

	public:
		/*! Show the window. */
		virtual void Show() = 0;

		/*! Hide the window. */
		virtual void Hide() = 0;

		/*! Raise the window to the top. */
		virtual void Raise() = 0;

		/*! Set window title. */
		virtual void SetTitle(const char* title) = 0;

		/*! Get size of the window. */
		virtual const FVector2& GetSize() const = 0;

		/*! Get position of the window. */
		virtual const FVector2& GetPosition() const = 0;

		/*! Get native window handle. */
		virtual RWindowHandle GetNativeHandle() = 0;

		/*! Get title of the window. */
		virtual const char* GetTitle() const = 0;

	protected:
		/*! Expose hidden functions to our windowing system. */
		friend class RWindowSystem;

		/*! Create window (internal) */
		virtual void Create() = 0;

		/*! Callback function when the window is created. */
		virtual void OnCreate() = 0;

		/*! Callback function when the window is resized. */
		virtual void OnResize(int width, int height) = 0;

		/*! Callback function when the window is moved. */
		virtual void OnMove(int x, int y) = 0;

		/*! Callback function when the window is focussed. */
		virtual void OnFocus() = 0;

		/*! Callback function when the window focus is lost. */
		virtual void OnFocusLost() = 0;

		/*! Callback function when the window posts a quit message. */
		virtual void OnClose() = 0;

	};
}

