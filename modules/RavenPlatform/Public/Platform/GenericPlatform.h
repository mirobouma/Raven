#pragma once
#include "Core/Service/Service.h"
#include "Platform/GenericPlatformWindow.h"
#include "Platform/WindowSystemInterface.h"

#include <cinttypes>
#include <memory>
#include <thread>

namespace Raven {

	/*!
	\enum EPlatformType
	\brief enumerator used to determine which platform we're running on.
	*/
	enum class EPlatformType {
		EPlatformType_Undefined = 0, /*!< Undefined platform. */
		EPlatformType_Windows, /*!< Windows based platform. */
		EPlatformType_Linux, /*!< Linux based platform. */
	};

	/*! 
	\class RGenericPlatformInfo
	\brief The RGenericPlatformInfo class is a generic implementation of platform-specific information.
	*/
	class RGenericPlatformInfo {
	public:
		RGenericPlatformInfo() = delete;
		RGenericPlatformInfo(const RGenericPlatformInfo&) = delete; /*!< Delete copy constuctor */
		RGenericPlatformInfo& operator= (const RGenericPlatformInfo&) = delete; /*!< Delete assignment constuctor */

		/*! Returns page size of this platform in KB.
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
		static EPlatformType GetPlatformType() { return EPlatformType::EPlatformType_Undefined; }

	};
}
