#pragma once
#include "Core/Utility/Enum.h"
#include "Math/Math.h"
#include "Math/Rect.h"
#include <string>

namespace Raven {
	typedef void* RWindowHandle; /*!< Represents handle to a platform window. */

	/*! 
	Enumerator to define flags for window creation. 
	*/
	enum EPlatformWindowFlags
	{
		EPlatformWindowFlags_None = 0, /*!< No special window creation flags specified. */
		EPlatformWindowFlags_Borderless = 1 << 0, /*!< Create the window without borders. */
		EPlatformWindowFlags_NoResize = 1 << 1, /*!< Do not allow the window to be resized. */
		EPlatformWindowFlags_Shown = 1 << 2, /*!< Show the window on creation by default. */
		EPlatformWindowFlags_AlwaysOnTop = 1 << 3, /*!< The window should always be on top. */
		EPlatformWindowFlags_AutoPosition = 1 << 4, /*!< Automatically choose location of the window. */
	};
	DECL_ENUM_AS_FLAGS(EPlatformWindowFlags);

	/*! 
	\class RGenericPlatformWindow 
	\brief Generic implementation of a platform window.
	*/
	class RGenericPlatformWindow {
	public:

		/*! Initialize the window with the specified position, location and flags.
		\param size size of the window.
		\param position position of the window, to automatically choose a position add the EPlatformWindowFlags_AutoPosition flag.
		\param flags Flags for window creation. 
		\sa EPlatformWindowFlags
		*/
		RGenericPlatformWindow(const char* title, Math::FVector2 size, Math::FVector2 position, EPlatformWindowFlags flags);
		RGenericPlatformWindow(); /*!< Initialize window with default location and size. */
		RGenericPlatformWindow(const RGenericPlatformWindow&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a window. */
		RGenericPlatformWindow& operator= (const RGenericPlatformWindow&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a window. */
		~RGenericPlatformWindow();

		/*! Show the window. */
		virtual void Show() {}

		/*! Hide the window. */
		virtual void Hide() {}

		/*! Raise the window to the top. */
		virtual void Raise() {}

		/*! Set window title. */
		virtual void SetTitle(const char* title) {}

		/*! Get native window handle. */
		virtual RWindowHandle GetNativeHandle() { return nullptr; }
		
		/*! Get size of the window. */
		inline const Math::FVector2& GetSize() const { return _geometry.Size; }

		/*! Get position of the window. */
		inline const Math::FVector2& GetPosition() const { return _geometry.Position; }

		/*! Get title of the window. */
		inline const char* GetTitle() const { return _title.c_str(); }

		/*! Returns if this window posted its quit message. */
		inline bool ShouldQuit() const { return _shouldQuit; }

	protected:
		/*! Friend window system interface so it can call the HandlewindowMessages method. */
		friend class RWindowSystemInterface;

		/*! Internal initialize function, should never be called directly. */
		void InternalInitialize(const char* title, Math::FRect geometry, EPlatformWindowFlags flags);

		/*! Handle window messages from the platform. */
		static void HandleWindowMessages() {};

		/*! Create window (internal) */
		virtual void Create() {};

		/*! Callback function when the window is resized. */
		virtual void OnResize(int width, int height) {};

		/*! Callback function when the window is moved. */
		virtual void OnMove(int x, int y) {};

		/*! Callback function when the window is focussed. */
		virtual void OnFocus() {};

		/*! Callback function when the window focus is lost. */
		virtual void OnFocusLost() {};

		/*! Callback function when the window posts a quit message. */
		virtual void OnQuit() { _shouldQuit = true; };

		std::string _title;
		Math::FRect _geometry; /*!< Internal representation of the window geometry. */
		EPlatformWindowFlags _flags; /*!< Internal cache of flags enabled on this window. */

		bool _shouldQuit; /*!< boolean if the window should quit.*/
	};
}
