#pragma once
#include "Core\System\System.h"

namespace Raven {
	/*!
	\class RWindowSystemInterface
	\brief Helper object to communicate platform-specific events such as resizing 
	and window creation to their respective abstracted windows.
	*/
	class RWindowSystemInterface : public ISystem {
	protected:
		RWindowSystemInterface() = default;
		RWindowSystemInterface(const RWindowSystemInterface&) = delete; /*!< Delete copy constuctor */
		RWindowSystemInterface& operator= (const RWindowSystemInterface&) = delete; /*!< Delete assignment constuctor */

		/*! Initialize the window system interface. */
		bool Initialize() final override;

		/*! Destroy the window system interface. */
		bool Destroy() final override;

	public:
		friend class IPlatform;
	};
}