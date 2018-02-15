#pragma once
#include "Core\System\System.h"
#include "Platform\PlatformWindow.h"
#include "Platform\WindowSystemInterface.h"

#include <cinttypes>

namespace Raven {
	/*! 
	The IPlatform class is an interface for platform-specific functionality.
	*/
	class IPlatform : public ISystem {
	public:
		IPlatform() = default;
		IPlatform(const IPlatform&) = delete; /*!< Delete copy constuctor */
		IPlatform& operator= (const IPlatform&) = delete; /*!< Delete assignment constuctor */

		/*! Initialize the platform interface.
		\return true if initialization succeeded.
		*/
		virtual bool Initialize() = 0; 

		/*! Destroy the platform interface.
		\return true if deallocation succeeded.
		*/
		virtual bool Destroy() = 0;

		/*! Create a platform window.
		\param width The width of the newly created window.
		\param height The height of the newly created window.
		\param title The title on the newly created window.
		\return handle to a native platform window.
		*/
		virtual IPlatformWindow* CreatePlatformWindow(uint32_t width, uint32_t height, const char* title, EPlatformWindowFlags flags = EPlatformWindowFlags::EPlatformWindowFlags_None) = 0;

	protected:

	};
}