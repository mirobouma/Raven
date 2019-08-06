#pragma once

namespace Raven {
	/*!
	\class RGenericPlatform
	\brief Static generic platform interface.
	*/
	class RGenericPlatform {
	protected:
		RGenericPlatform() = delete; /*!< This is a pure static class so we don't have an constructor. */
		RGenericPlatform(const RGenericPlatform&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a platform. */
		RGenericPlatform& operator= (const RGenericPlatform&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a platform. */

	public:
		/*! Initializes this platform. */
		static bool Initialize() {};

		/*! Initializes this platform. */
		static bool Destroy() {};
	};

	typedef RGenericPlatform RPlatform;
}