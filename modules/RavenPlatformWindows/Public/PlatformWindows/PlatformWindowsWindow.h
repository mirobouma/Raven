#pragma once
#include "Platform\GenericPlatformWindow.h"

namespace Raven {
	/*!
	\class RPlatformWindowWin32
	\brief Win32 Native window handle.
	*/
	class RPlatformWindowWin32 : public IGenericPlatformWindow {
	protected:
		RPlatformWindowWin32() = default;
	public:
		friend class RPlatformWin32;
		friend class RWindowSystemInterface;
	};
}