#pragma once

#include "HAL/PlatformThreading.h"

namespace Raven
{
	/*! \class RFiber
	Wrapper around RPlatformFiber.
	*/
	class RFiber : public RPlatformFiber {
	public:
		RFiber() : RPlatformFiber() {}
		RFiber(RFiberStart startRoutine, void *arg, std::size_t stackSize = 0) : RPlatformFiber(stackSize, startRoutine, arg) { }
	};
}