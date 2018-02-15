#pragma once

namespace Raven {
	/*!
	\class WindowSystemInterface
	\brief Helper object to communicate platform-specific events such as resizing 
	and window creation to their respective abstracted windows.
	*/
	class WindowSystemInterface {
	protected:
		WindowSystemInterface() = default;
	public:
		friend class IPlatform;
	};
}