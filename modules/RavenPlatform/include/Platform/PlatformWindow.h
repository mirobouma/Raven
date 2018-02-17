#pragma once
#include "Core\Utility\Enum.h"
#include "Math\Math.h"
#include "Math\Rect.h"

namespace Raven {
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
	\class IPlatformWindow 
	\brief Interface for a native window. 
	*/
	class IPlatformWindow {
	protected:
		IPlatformWindow() = default;
	public:
		friend class IPlatform;
		friend class RWindowSystemInterface;
	protected:
		Math::FRect _geometry;
		EPlatformWindowFlags _flags; /*!< Internal cache of flags enabled on this window. */
	};
}