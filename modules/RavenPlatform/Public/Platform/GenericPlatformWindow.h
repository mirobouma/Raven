#pragma once
#include "Core/Utility/Enum.h"
#include "Math/Math.h"
#include "Math/Rect.h"

namespace Raven {
	typedef int32_t RWindowHandle; /*!< Represents handle to a platform window. */

	/*! 
	Enumerator to define flags for window creation. 
	*/
	enum class EPlatformWindowFlags
	{
		EPlatformWindowFlags_None = 0, /*!< No special window creation flags specified. */
		EPlatformWindowFlags_Borderless = 1 << 0, /*!< Create the window without borders. */
		EPlatformWindowFlags_NoResize = 1 << 1, /*!< Do not allow the window to be resized. */
	};
	DECL_ENUM_AS_FLAGS(EPlatformWindowFlags);

	/*! 
	\class RGenericPlatformWindow 
	\brief Generic implementation of a platform window.
	*/
	class RGenericPlatformWindow {
	public:
		friend class RWindowSystemInterface;

		RGenericPlatformWindow() = default;
		RGenericPlatformWindow(const RGenericPlatformWindow&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a window. */
		RGenericPlatformWindow& operator= (const RGenericPlatformWindow&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a window. */

		/*!
		\brief Callback function when the window is resized.
		*/
		virtual void OnResize(int width, int height) = 0;

	protected:
		Math::FRect _geometry;
		EPlatformWindowFlags _flags; /*!< Internal cache of flags enabled on this window. */
	};
}
