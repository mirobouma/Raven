#pragma once
#include <cinttypes>

namespace Raven
{
	/*! \struct IPlatformThreadHandle
	\brief Wrapper structure around platform-specific thread handle.
	*/
	class INativeThreadHandle {};

	/*! Entry point typdef. */
	typedef unsigned int(_stdcall NativeThreadStart)(void* arg);

	/*! \class INativeThread
	\brief Interface for platform implementation of a fiber.
	*/
	class INativeThread
	{
	public:
		INativeThread() = default;

		/*! Fill this object with parameters of the currently running thread. */
		virtual INativeThread* from_current_thread() = 0;

		/*! Create the platform thread. */
		virtual bool create(uint32_t stackSize, NativeThreadStart* start, void* arg, size_t coreAffinity = -1) = 0;

		/*! Join the thread. */
		virtual void join() = 0;

		/*! Set thread affinity. */
		virtual void set_affinity(size_t affinity) = 0;

		/*! Get platform-specific handle for this thread. */
		virtual INativeThreadHandle* get_handle() = 0;

		/*! Get unique ID for this thread. */
		virtual uint64_t get_id() = 0;

		/*! Checks if this thread equals the currently running thread. */
		virtual bool is_current_thread() const = 0;
	};
}