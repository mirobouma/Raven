#pragma once
#include "Platform/Defines.h"
#include <cinttypes>

namespace Raven {
	/*! 
	\class RPlatformWindowsInfo
	\brief Windows implementation of retrieval of misscelanious platform information. 
	*/
	class RPlatformWindowsInfo {
	protected:
		RPlatformWindowsInfo() = default;
		RPlatformWindowsInfo(const RPlatformWindowsInfo&) = delete; /*!< Delete copy constuctor */
		RPlatformWindowsInfo& operator= (const RPlatformWindowsInfo&) = delete; /*!< Delete assignment constuctor */

	public:
		/*! Returns page size of this platform.
		\details Used in custom memory allocators to define the optimal allocation size for pages.
		*/
		static uint32_t GetPageSize();

		/*! Retrieve the platform type (Win32).
		\returns EPlatform::EPlatform_Win32;
		\sa EPlatform
		*/
		static constexpr EPlatformType GetPlatformType() { return EPlatformType::EPlatformType_Windows; }
	};

	// HAL abstraction
	typedef RPlatformWindowsInfo RPlatformInfo;
};