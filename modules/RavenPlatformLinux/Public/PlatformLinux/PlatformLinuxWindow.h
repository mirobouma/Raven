#pragma once
#include "Platform/Interfaces/IPlatformWindow.h"

struct SDL_Window;

namespace Raven {
	/*!
	\class RPlatformLinuxWindow
	\brief Linux platform window.
	*/
	class RPlatformLinuxWindow : public IPlatformWindow {
	public:
		/*! Initialize the window with the specified position, location and flags.
		\param size size of the window.
		\param position position of the window, to automatically choose a position add the EPlatformWindowFlags_AutoPosition flag.
		\param flags Flags for window creation.
		\sa EPlatformWindowFlags
		*/
		RPlatformLinuxWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler);
		RPlatformLinuxWindow(IWindowMessageHandler* handler); /*!< Initialize window with default location and size. */
		
		/*! Show the window. */
		virtual void Show() override;

		/*! Hide the window. */
		virtual void Hide() override;

		/*! Raise the window to the top. */
		virtual void Raise() override;

		/*! Set window title. */
		virtual void SetTitle(const char* title);

		/*! Get size of the window. */
		virtual const Math::FVector2& GetSize() const { return _geometry.Size; }

		/*! Get position of the window. */
		virtual const Math::FVector2& GetPosition() const { return _geometry.Position; }

		/*! Get title of the window. */
		virtual const char* GetTitle() const { return _title.c_str(); }
		
		/*! Get native window handle. */
		virtual RWindowHandle GetNativeHandle() override;

	protected:
		/*! Expose hidden functions to our windowing system. */
		friend class RWindowSystem;

		/*! Internal initialize function, should never be called directly. */
		void InternalInitialize(const char* title, Math::FRect geometry, EPlatformWindowFlags flags);

		/*! Handle window messages from the platform. */
		static void HandleWindowMessages();

		/*! Create window (internal) */
		virtual void Create() override;

		/*! Callback function when the window is created. */
		virtual void OnCreate() override;

		/*! Callback function when the window is resized. */
		virtual void OnResize(int width, int height) override;

		/*! Callback function when the window is moved. */
		virtual void OnMove(int x, int y) override;

		/*! Callback function when the window is focussed. */
		virtual void OnFocus() override;

		/*! Callback function when the window focus is lost. */
		virtual void OnFocusLost() override;

		/*! Callback function when the window posts a quit message. */
		virtual void OnClose() override {};

		/*! Internal handle to the window. */
		SDL_Window* _window;

		std::string _title; /*!< Title of the window. */
		Math::FRect _geometry; /*!< Internal representation of the window geometry. */
		EPlatformWindowFlags _flags; /*!< Internal cache of flags enabled on this window. */
		IWindowMessageHandler* _handler; /*!< Internal reference to the window message handler associated with this window. */
	};

	typedef RPlatformLinuxWindow RPlatformWindow;
}
