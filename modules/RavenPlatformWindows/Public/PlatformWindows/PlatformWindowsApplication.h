#pragma once
#include "Platform/GenericPlatformApplication.h"

namespace Raven {
	/*!
	\class RPlatformWindowsApplication
	\brief Windows implementation of the platform interface.
	*/
	class RPlatformWindowsApplication : public RGenericPlatformApplication {
	public:
		RPlatformWindowsApplication() = default;
		RPlatformWindowsApplication(const RPlatformWindowsApplication&) = delete; /*!< Delete copy constuctor */
		RPlatformWindowsApplication& operator= (const RPlatformWindowsApplication&) = delete; /*!< Delete assignment constuctor */

		/*! Initializes this platform application and required subsystems. */
		virtual bool Initialize() override;

		/*! Initializes this platform application and required subsystems. */
		virtual bool Destroy() override;

		/*! Start application loop. */
		virtual void Start() override;
	};

	// HAL abstraction
	typedef RPlatformWindowsApplication RApplication;
};