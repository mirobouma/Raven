#pragma once
#include "Platform/GenericPlatformWindow.h"

namespace Raven {
	/*!
	\class RPlatformWindowsWindow
	\brief Native windows window.
	*/
	class RPlatformWindowsWindow : public RGenericPlatformWindow {
	public:
		RPlatformWindowsWindow() = default;

		/*! Show Windows window. */
		virtual void Show() override;
	};

	typedef RPlatformWindowsWindow RPlatformWindow;
}
