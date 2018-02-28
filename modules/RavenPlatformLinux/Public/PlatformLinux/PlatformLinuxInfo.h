#pragma once
#include "Platform/GenericPlatformInfo.h"

namespace Raven {
	/*! 
	\class RPlatformLinuxInfo
	\brief Linux implementation of the platform interface. 
	*/
	class RPlatformLinuxInfo : public RGenericPlatformInfo {

		RPlatformLinuxInfo() = default;
		RPlatformLinuxInfo(const RPlatformLinuxInfo&) = delete; /*!< Delete copy constuctor */
		RPlatformLinuxInfo& operator= (const RPlatformLinuxInfo&) = delete; /*!< Delete assignment constuctor */

		/*! Returns page size of this platform.
		\details Used in custom memory allocators to define the optimal allocation size for pages.
		*/
		static uint32_t GetPageSize();

		/*! Retrieve the platform type (Linux).
		\returns EPlatformType::EPlatformType_Linux;
		\sa EPlatformType
		*/
		static constexpr EPlatformType GetPlatformType() { return EPlatformType::EPlatformType_Linux; }

	};

	// HAL abstraction
	typedef RPlatformLinuxInfo RPlatformInfo;
};