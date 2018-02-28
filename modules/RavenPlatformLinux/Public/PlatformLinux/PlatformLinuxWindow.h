#pragma once
#include "Platform/GenericPlatformWindow.h"

namespace Raven {
	/*!
	\class RPlatformLinuxWindow
	\brief Linux platform window.
	*/
	class RPlatformLinuxWindow: public RGenericPlatformWindow {
	public:
		RPlatformLinuxWindow() = default;
		
		/*! Show linux window. */
		virtual void Show() override;
	};

	typedef RPlatformLinuxWindow RPlatformWindow;
}
