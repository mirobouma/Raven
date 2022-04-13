#pragma once
#include "platform/defines.h"
#include <cinttypes>
#include <thread>

namespace Raven 
{
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

		/*! Ask the platform for the number of available logical processors.
		\return Amount of available logical processor cores.
		*/
		static inline uint32_t GetHardwareConcurrency() { return std::thread::hardware_concurrency(); }

		/*! Retrieve the platform type (Win32).
		\returns EPlatform::EPlatform_Win32;
		\sa EPlatform
		*/
		static constexpr EPlatformType GetPlatformType() { return EPlatformType::EPlatformType_Windows; }
	};

	// HAL abstraction
	typedef RPlatformWindowsInfo RPlatformInfo;
};