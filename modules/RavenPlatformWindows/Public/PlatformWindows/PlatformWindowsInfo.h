#pragma once
#include "Platform/GenericPlatform.h"

namespace Raven {
	/*! 
	\class RPlatformWindowsInfo
	\brief Windows implementation of retrieval of misscelanious platform information. 
	*/
	class RPlatformWindowsInfo : public RGenericPlatformInfo {

		RPlatformWindowsInfo() = default;
		RPlatformWindowsInfo(const RPlatformWindowsInfo&) = delete; /*!< Delete copy constuctor */
		RPlatformWindowsInfo& operator= (const RPlatformWindowsInfo&) = delete; /*!< Delete assignment constuctor */

		/*! Returns page size of this platform.
		\details Used in custom memory allocators to define the optimal allocation size for pages.
		*/
		static uint32_t GetPageSize();

		/*! Retrieve the platform type (Win32).
		\returns EPlatform::EPlatform_Win32;
		\sa EPlatform
		*/
		static EPlatformType GetPlatformType() { return EPlatformType::EPlatformType_Windows; }
	};

	// HAL abstraction
	typedef RPlatformWindowsInfo RPlatformInfo;
};