#pragma once
#include "Platform/GenericPlatform.h"

namespace Raven {
	/*! 
	\class RPlatformWin32
	\brief Windows (Win32) implementation of the platform interface. 
	*/
	class RPlatformWin32 : public IGenericPlatformInfo {

		RPlatformWin32() = default;
		RPlatformWin32(const RPlatformWin32&) = delete; /*!< Delete copy constuctor */
		RPlatformWin32& operator= (const RPlatformWin32&) = delete; /*!< Delete assignment constuctor */

		/*! Initialize the platform interface.
		\return true if initialization succeeded.
		*/
		virtual bool Initialize();

		/*! Destroy the platform interface.
		\return true if deallocation succeeded.
		*/
		virtual bool Destroy() = 0;

		/*! Returns page size of this platform.
		\details Used in custom memory allocators to define the optimal allocation size for pages.
		*/
		virtual uint32_t GetPageSize();

		/*! Retrieve the platform type (Win32).
		\returns EPlatform::EPlatform_Win32;
		\sa EPlatform
		*/
		virtual EPlatform GetPlatformType() const override { return EPlatform::EPlatform_Win32; }
	};
};