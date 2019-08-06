#pragma once

namespace Raven
{
	/*! \struct IPlatformThreadHandle
	\brief Wrapper structure around platform-specific thread handle.
	*/
	struct IPlatformThreadHandle {};

	/*! Entry point typdef. */
	typedef unsigned int (_stdcall RPlatformThreadStart)(void *arg);

	/*! Entry point for fibers typdef. */
	typedef void (*RFiberStart)(void *arg);

	/*! \class IPlatformFiber
	\brief Interface for platform implementation of a fiber.
	*/
	class IPlatformThread
	{
	public:
		IPlatformThread() = default;

		/*! Fill this object with parameters of the currently running thread. */
		virtual IPlatformThread* FromCurrentThread() = 0;

		/*! Create the platform thread. */
		virtual bool Create(uint32_t stackSize, RPlatformThreadStart* start, void *arg, size_t coreAffinity = -1) = 0;

		/*! Join the thread. */
		virtual void Join() = 0;

		/*! Set thread affinity. */
		virtual void SetAffinity(size_t affinity) = 0;

		/*! Get platform-specific handle for this thread. */
		virtual IPlatformThreadHandle* GetHandle() = 0;

		/*! Get unique ID for this thread. */
		virtual uint64_t GetID() = 0;

		/*! Checks if this thread equals the currently running thread. */
		virtual bool IsCurrentThread() const = 0;
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