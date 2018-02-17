#pragma once
#include "Platform\PlatformWindow.h"

namespace Raven {
	/*!
	\class RPlatformWindowWin32
	\brief Win32 Native window handle.
	*/
	class RPlatformWindowWin32 : public IPlatformWindow {
	protected:
		RPlatformWindowWin32() = default;
	public:
		friend class RPlatformWin32;
		friend class RWindowSystemInterface;
	};
}