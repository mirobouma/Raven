#pragma once
#include "Core\System\System.h"
#include "Platform\PlatformWindow.h"
#include "Platform\WindowSystemInterface.h"

#include <cinttypes>
#include <memory>
#include <thread>

namespace Raven {

	/*!
	\enum EPlatform
	\brief enumerator used to determine which platform we're running on.
	*/
	enum class EPlatform {
		EPlatform_Undefined = 0, /*!< Undefined platform. */
		EPlatform_Win32, /*!< Win32 based platform. */
		EPlatform_UWP, /*!< Unsupported */
	};

	/*! 
	\class IPlatform
	\brief The IPlatform class is an interface for platform-specific functionality.
	*/
	class IPlatform : public ISystem {
	public:
		IPlatform() = default;
		IPlatform(const IPlatform&) = delete; /*!< Delete copy constuctor */
		IPlatform& operator= (const IPlatform&) = delete; /*!< Delete assignment constuctor */

		/*! Initialize the platform interface.
		\return true if initialization succeeded.
		*/
		virtual bool Initialize(); 

		/*! Destroy the platform interface.
		\return true if deallocation succeeded.
		*/
		virtual bool Destroy() = 0;

		/*! Create a platform window.
		\param width The width of the newly created window.
		\param height The height of the newly created window.
		\param title The title on the newly created window.
		\param flags Window creation flags.
		\sa EPlatformWindowFlags
		\return handle to a native platform window.
		*/
		virtual IPlatformWindow* CreatePlatformWindow(uint32_t width, uint32_t height, const char* title, EPlatformWindowFlags flags = EPlatformWindowFlags::EPlatformWindowFlags_None) = 0;

		/*! Get a pointer to the window system interface. */
		inline const RWindowSystemInterface* GetWindowSystemInterface() const { return _WindowSystemInterface.get(); }

		/*! Returns page size of this platform.
		\details Used in custom memory allocators to define the optimal allocation size for pages.
		*/
		virtual uint32_t GetPageSize() const = 0;

		/*! Ask the platform for the number of available logical processors.
		\return Amount of available logical processor cores.
		*/
		inline uint32_t GetHardwareConcurrency() const { return std::thread::hardware_concurrency(); }

		/*! Retrieve the platform type.
		\returns enum of the currently selected platform.
		\sa EPlatform
		*/
		virtual EPlatform GetPlatformType() const { return EPlatform::EPlatform_Undefined; }

	protected:
		std::unique_ptr<RWindowSystemInterface> _WindowSystemInterface; /*!< Window system interface. */
	};
}