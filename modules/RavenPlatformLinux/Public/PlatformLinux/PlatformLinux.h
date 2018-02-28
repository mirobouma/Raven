#pragma once
#include "Platform/GenericPlatform.h"

namespace Raven {
	/*!
	\class RPlatformLinux
	\brief Linux platform interface.
	*/
	class RPlatformLinux : public RGenericPlatform {
	protected:
		RPlatformLinux() = default;
		RPlatformLinux(const RPlatformLinux&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a platform. */
		RPlatformLinux& operator= (const RPlatformLinux&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a platform. */

	public:
		/*! Initializes this platform. */
		static bool Initialize();

		/*! Initializes this platform. */
		static bool Destroy();
	};

	typedef RPlatformLinux RPlatform;
}