#pragma once
#include "Platform/GenericPlatformWindow.h"

namespace Raven {
	/*!
	\class RPlatformWindowLinux
	\brief Linux platform window.
	*/
	class RPlatformWindowLinux : public RGenericPlatformWindow {
	protected:
		RPlatformWindowLinux() = default;
	public:
		friend class RWindowSystemInterface;
	};
}
