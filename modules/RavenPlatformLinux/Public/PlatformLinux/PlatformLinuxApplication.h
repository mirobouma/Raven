#pragma once
#include "Platform/GenericPlatformApplication.h"

namespace Raven {
	/*!
	\class RPlatformLinuxApplication
	\brief Linux implementation of the platform interface.
	*/
	class RPlatformLinuxApplication : public RGenericPlatformApplication {
	public:
		RPlatformLinuxApplication() = default;
		RPlatformLinuxApplication(const RPlatformLinuxApplication&) = delete; /*!< Delete copy constuctor */
		RPlatformLinuxApplication& operator= (const RPlatformLinuxApplication&) = delete; /*!< Delete assignment constuctor */

		/*! Initializes this platform application and required subsystems. */
		virtual bool Initialize() override;

		/*! Initializes this platform application and required subsystems. */
		virtual bool Destroy() override;

	};

	// HAL abstraction
	typedef RPlatformLinuxApplication RApplication;
};