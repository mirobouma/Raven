#pragma once

namespace Raven
{
	/*! \struct IPlatformThreadHandle
	\brief Wrapper structure around platform-specific thread handle.
	*/
	struct IPlatformThreadHandle {};

	/*! Entry point typdef. */
	typedef unsigned int (RThreadStart)(void *arg);

	/*! Entry point for fibers typdef. */
	typedef void (*RFiberStart)(void *arg);

	/*! \class IPlatformFiber
	\brief Interface for platform implementation of a fiber.
	*/
	class IPlatformThread
	{
	public:
		IPlatformThread() = default;
		IPlatformThread(const IPlatformThread&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a thread. */
		IPlatformThread& operator= (const IPlatformThread&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a thread. */

		/*! Create the platform thread. */
		virtual bool Create(uint32_t stackSize, RThreadStart* start, void *arg, size_t coreAffinity = -1) = 0;

		/*! Join the thread. */
		virtual void Join() = 0;

		/*! Set thread affinity. */
		virtual void SetAffinity(size_t affinity) = 0;

		/*! Get platform-specific handle for this thread. */
		virtual IPlatformThreadHandle* GetHandle() = 0;

	};

	/*! \class IPlatformFiber
	\brief Interface for platform implementation of a fiber.
	*/
	class IPlatformFiber
	{
	public:
		/*! Switch to different fiber. */
		virtual void SwitchToFiber(IPlatformFiber* fiber) = 0;

		/*! Reset fiber. */
		virtual void Reset(RFiberStart startRoutine, void *arg) = 0;

	};
}