#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Raven {
	/*!
	\class RPlatformWindows
	\brief Windows platform interface.
	*/
	class RPlatformWindows {
	protected:
		RPlatformWindows() = default;
		RPlatformWindows(const RPlatformWindows&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a platform. */
		RPlatformWindows& operator= (const RPlatformWindows&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a platform. */

	public:
		/*! Initializes this platform. */
		static bool Initialize();

		/*! Initializes this platform. */
		static bool Destroy();
	};

	typedef RPlatformWindows RPlatform;
}