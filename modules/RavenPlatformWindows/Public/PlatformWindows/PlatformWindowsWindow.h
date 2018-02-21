#pragma once
#include "Platform/GenericPlatformWindow.h"

namespace Raven {
	/*!
	\class RPlatformWindowWindows
	\brief Native windows window.
	*/
	class RPlatformWindowWindows : public RGenericPlatformWindow {
	protected:
		RPlatformWindowWindows() = default;
	public:
		friend class RWindowSystemInterface;
	};
}
