#pragma once

namespace Raven
{
	/*! Entry point for fibers typdef. */
	typedef void (*NativeFiberStart)(void* arg);
	/*! \class IPlatformFiber
	\brief Interface for platform implementation of a fiber.
	*/
	class IPlatformFiber
	{
	public:
		/*! Switch to different fiber. */
		virtual void SwitchToFiber(IPlatformFiber* fiber) = 0;

		/*! Reset fiber. */
		virtual void Reset(NativeFiberStart startRoutine, void* arg) = 0;
	};
}
