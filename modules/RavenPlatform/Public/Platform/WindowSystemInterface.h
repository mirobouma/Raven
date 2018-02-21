#pragma once
#include "Core/Service/Service.h"
#include "Platform/GenericPlatformWindow.h"

namespace Raven {
	/*!
	\class RWindowSystemInterface
	\brief Helper object to communicate platform-specific events such as resizing 
	and window creation to their respective abstracted windows.
	*/
	class RWindowSystemInterface : public IService<RWindowSystemInterface> {
	protected:
		RWindowSystemInterface() = default;
		RWindowSystemInterface(const RWindowSystemInterface&) = delete; /*!< Delete copy constuctor */
		RWindowSystemInterface& operator= (const RWindowSystemInterface&) = delete; /*!< Delete assignment constuctor */

		/*! Initialize the window system interface. */
		bool Initialize() final override;

		/*! Destroy the window system interface. */
		bool Destroy() final override;

		/*! Register Platform Window to window system interface. */
		RGenericPlatformWindow* RegisterPlatformWindow(RGenericPlatformWindow* Window) {};

		/*! Register Platform Window to window system interface. */
		bool UnregisterPlatformWindow(RGenericPlatformWindow* Window) {};

	public:
		friend class REngine;
	};
}
