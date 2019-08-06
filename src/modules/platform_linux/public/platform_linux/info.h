#pragma once
#include <cinttypes>
#include "platform/defines.h"
#include <thread>

namespace Raven {
	/*! 
	\class RPlatformLinuxInfo
	\brief The RPlatformLinuxInfo class is a linux implementation of platform-specific information.
	*/
	class RPlatformLinuxInfo
	{
		RPlatformLinuxInfo() = delete; /*!< Pure static class, so can't instantiate. */
		RPlatformLinuxInfo(const RPlatformLinuxInfo&) = delete; /*!< Delete copy constuctor */
		RPlatformLinuxInfo& operator= (const RPlatformLinuxInfo&) = delete; /*!< Delete assignment constuctor */
	public:
		/*! Returns page size of this platform.
		\details Used in custom memory allocators to define the optimal allocation size for pages.
		*/
		static uint32_t GetPageSize();

		/*! Ask the platform for the number of available logical processors.
		\return Amount of available logical processor cores.
		*/
		static inline uint32_t GetHardwareConcurrency() { return std::thread::hardware_concurrency(); }

		/*! Retrieve the platform type (Linux).
		\returns EPlatformType::EPlatformType_Linux;
		\sa EPlatformType
		*/
		static constexpr EPlatformType GetPlatformType() { return EPlatformType::EPlatformType_Linux; }

	};

	// HAL abstraction
	typedef RPlatformLinuxInfo RPlatformInfo;
};