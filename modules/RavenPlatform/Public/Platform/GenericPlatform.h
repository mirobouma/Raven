#pragma once

namespace Raven {
	/*!
	\class RGenericPlatform
	\brief Generic platform interface.
	*/
	class RGenericPlatform {
	protected:
		RGenericPlatform() = default;
		RGenericPlatform(const RGenericPlatform&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a platform. */
		RGenericPlatform& operator= (const RGenericPlatform&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a platform. */

	public:
		/*! Initializes this platform. */
		static bool Initialize() {};

		/*! Initializes this platform. */
		static bool Destroy() {};
	};
}