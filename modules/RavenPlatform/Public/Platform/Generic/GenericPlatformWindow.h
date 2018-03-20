#pragma once
#include "Core/Utility/Enum.h"
#include "Platform/Interfaces/IPlatformWindow.h"

namespace Raven {
	
	/*! 
	\class RGenericPlatformWindow 
	\brief Generic implementation of a platform window.
	*/
	class RGenericPlatformWindow : public IPlatformWindow {
	public:

		/*! Initialize the window with the specified position, location and flags.
		\param size size of the window.
		\param position position of the window, to automatically choose a position add the EPlatformWindowFlags_AutoPosition flag.
		\param flags Flags for window creation. 
		\sa EPlatformWindowFlags
		*/
		RGenericPlatformWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags, IWindowMessageHandler* handler);
		RGenericPlatformWindow(IWindowMessageHandler* handler); /*!< Initialize window with default location and size. */
		RGenericPlatformWindow(const RGenericPlatformWindow&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a window. */
		RGenericPlatformWindow& operator= (const RGenericPlatformWindow&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a window. */
		~RGenericPlatformWindow();

		/*! Show the window. */
		virtual void Show() override final {}

		/*! Hide the window. */
		virtual void Hide() override final {}

		/*! Raise the window to the top. */
		virtual void Raise() override final {}

		/*! Set window title. */
		virtual void SetTitle(const char* title) override final {}

		/*! Get size of the window. */
		virtual const Math::FVector2& GetSize() const override final { return _geometry.Size; }

		/*! Get position of the window. */
		virtual const Math::FVector2& GetPosition() const override final { return _geometry.Position; }

		/*! Get native window handle. */
		virtual RWindowHandle GetNativeHandle() override final { return nullptr; }

		/*! Get title of the window. */
		virtual const char* GetTitle() const override final { return _title.c_str(); }

	protected:
		/*! Expose hidden functions to our windowing system. */
		friend class RWindowSystem;

		/*! Internal initialize function, should never be called directly. */
		void InternalInitialize(const char* title, Math::FRect geometry, EPlatformWindowFlags flags);

		/*! Handle window messages from the platform. */
		static void HandleWindowMessages() {};

		/*! Create window (internal) */
		virtual void Create() override final;

		/*! Callback function when the window is focussed. */
		virtual void OnCreate() {};

		/*! Callback function when the window is resized. */
		virtual void OnResize(int width, int height) {};

		/*! Callback function when the window is moved. */
		virtual void OnMove(int x, int y) {};

		/*! Callback function when the window is focussed. */
		virtual void OnFocus() {};

		/*! Callback function when the window focus is lost. */
		virtual void OnFocusLost() {};

		/*! Callback function when the window posts a quit message. */
		virtual void OnClose() {};

		std::string _title;
		Math::FRect _geometry; /*!< Internal representation of the window geometry. */
		EPlatformWindowFlags _flags; /*!< Internal cache of flags enabled on this window. */
		IWindowMessageHandler* _handler; /*!< Internal reference to the window message handler associated with this window. */
	};

	// HAL abstraction
	typedef RGenericPlatformWindow RPlatformWindow;
}
