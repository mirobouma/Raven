#pragma once
#include "Core/Utility/Enum.h"
#include "Math/Math.h"
#include "Math/Rect.h"

namespace Raven {
	/*!
	\class RGenericPlatformWindow
	\brief Generic implementation of an application.
	*/
	class RGenericPlatformApplication {
	public:
		RGenericPlatformApplication() = default;
		RGenericPlatformApplication(const RGenericPlatformApplication&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a window. */
		RGenericPlatformApplication& operator= (const RGenericPlatformApplication&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a window. */
	};
}
