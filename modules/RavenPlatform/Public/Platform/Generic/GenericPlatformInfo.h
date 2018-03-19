#pragma once
#include "Platform/WindowSystemInterface.h"

#include <cinttypes>
#include <memory>
#include <thread>

#include "Platform/Defines.h"

namespace Raven {

	/*! 
	\class RGenericPlatformInfo
	\brief The RGenericPlatformInfo class is a generic implementation of platform-specific information.
	*/
	class RGenericPlatformInfo {
	protected:
		RGenericPlatformInfo() = delete; /*!< Pure static class, so can't instantiate. */
		RGenericPlatformInfo(const RGenericPlatformInfo&) = delete; /*!< Delete copy constuctor. */
		RGenericPlatformInfo& operator= (const RGenericPlatformInfo&) = delete; /*!< Delete assignment constuctor. */

	public:

		/*! Returns page size of this platform in kb (2mb by default).
		\details Used in custom memory allocators to define the optimal allocation size for pages.
		*/
		static uint32_t GetPageSize() { return 2048; };

		/*! Ask the platform for the number of available logical processors.
		\return Amount of available logical processor cores.
		*/
		static inline uint32_t GetHardwareConcurrency() { return std::thread::hardware_concurrency(); }

		/*! Retrieve the platform type.
		\returns enum of the currently selected platform.
		\sa EPlatform
		*/
		static constexpr EPlatformType GetPlatformType() { return EPlatformType::EPlatformType_Undefined; }
	};

	// HAL abstraction
	typedef RGenericPlatformInfo RPlatformInfo;
}
