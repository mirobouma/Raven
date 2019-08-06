#pragma once
#include <cinttypes>
#include <memory>
#include <thread>
#include <string>
#include "platform/defines.h"
#include <vector>

namespace Raven {

	/*!
	\class RGenericPlatformFileSystem
	\brief The RGenericPlatformFileSystem class is a generic file system implementation.
	*/
	class RGenericPlatformFileSystem {
	protected:
		RGenericPlatformFileSystem() = delete; /*!< Pure static class, so can't instantiate. */
		RGenericPlatformFileSystem(const RGenericPlatformFileSystem&) = delete; /*!< Delete copy constuctor. */
		RGenericPlatformFileSystem& operator= (const RGenericPlatformFileSystem&) = delete; /*!< Delete assignment constuctor. */

	public:
		/*! Read a file and return a vector of it's content. */
		static std::vector<char> ReadFile(const std::string& filename);
	};

	// HAL abstraction
	typedef RGenericPlatformFileSystem RPlatformFileSystem;
}
