#pragma once
#include "platform/interfaces/window.h"
#include <memory>

namespace Raven {
	
	// Forward declare pimpl handle.
	struct WindowsWinHandle;

	// Required for pimpl. Defining a custom deleter function for unique_ptr.
	struct WindowsWinHandleDeleter
	{
		void operator()(WindowsWinHandle *p);
	};

	/*!
	\class RPlatformWindowsWindow
	\brief Native windows window. Should be used as a base class for specialized windows using the platform-independent RPlatformWindow typdef.
	\sa RPlatformWindow
	*/
	class RPlatformWindowsWindow : public IPlatformWindow {
	protected:
		/*! Initialize the window with the specified position, location and flags.
		\param size size of the window.
		\param position position of the window, to automatically choose a position add the EPlatformWindowFlags_AutoPosition flag.
		\param flags Flags for window creation.
		\sa EPlatformWindowFlags
		*/
		RPlatformWindowsWindow(const char* title, FVector2 size, FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler);
		RPlatformWindowsWindow(IWindowMessageHandler* handler); /*!< Initialize window with default location and size. */
		~RPlatformWindowsWindow() {}

	public:
		/*! Show the window. */
		virtual void Show() override final;

		/*! Hide the window. */
		virtual void Hide() override final;

		/*! Raise the window to the top. */
		virtual void Raise() override final;

		/*! Set window title. */
		virtual void SetTitle(const char* title) override final;

		/*! Get size of the window. */
		virtual const FVector2& GetSize() const final { return _geometry.Size; }

		/*! Get position of the window. */
		virtual const FVector2& GetPosition() const final { return _geometry.Position; }

		/*! Get title of the window. */
		virtual const char* GetTitle() const final { return _title.c_str(); }

		/*! Get native window handle. */
		virtual RWindowHandle GetNativeHandle() override final;

		/*! Get message handler (required for windows message handling). */
		IWindowMessageHandler* GetMessageHandler() const { return _handler; }

	protected:
		/*! Expose hidden functions to our windowing system. */
		friend class RWindowSystem;

		/*! Internal initialize function, should never be called directly. */
		void InternalInitialize(const char* title, FRect geometry, EPlatformWindowFlags flags);

		/*! Handle window messages from the platform. */
		static void HandleWindowMessages();

		/*! Create window (internal) */
		virtual void Create() override final;

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

		/*! Internal handle to the window. Using Pimpl idiom to hide windows-specific implementation. */
		std::unique_ptr<WindowsWinHandle, WindowsWinHandleDeleter> _winHandle;

		std::string _title; /*!< Title of the window. */
		FRect _geometry; /*!< Internal representation of the window geometry. */
		EPlatformWindowFlags _flags; /*!< Internal cache of flags enabled on this window. */
		IWindowMessageHandler* _handler; /*!< Internal reference to the window message handler associated with this window. */
	};

	typedef RPlatformWindowsWindow RPlatformWindow;
}
