#pragma once
#include "HAL/PlatformThreading.h"
#include "Core/Threading/Task.h"
namespace Raven
{
	/*! \class RThread
	\brief Wrapper around RPlatformThread for convenience of usage of std::function and emulated TLS.
	*/
	class RThread {
	public:
		/*! Start args. */
		struct FThreadStartArgs 
		{
			RThread*						mThread;
			std::function<void(RThread*)>	mFunc;
		};

		RThread(std::function<void(RThread*)> func)
		{
			_mArgs.mFunc = func;
			_mThread.Create(0, RThread::StartThread, (void*)&_mArgs);
		}

		static uint32_t StartThread(void* args)
		{
			FThreadStartArgs* arg = (FThreadStartArgs*)args;
			arg->mFunc(arg->mThread);

			return 0;
		}

	protected:
		FThreadStartArgs _mArgs;
		RPlatformThread _mThread;
	};
}