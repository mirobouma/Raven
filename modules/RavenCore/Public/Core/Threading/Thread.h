#pragma once
#include "HAL/PlatformThreading.h"
#include "Core/Threading/Task.h"
namespace Raven
{
	/*! Entry point typdef. */
	typedef void(*RThreadStart)(void *arg);

	/*! \class RThread
	\brief Wrapper around RPlatformThread for convenience.
	*/
	class RThread {
	public:
		/*! Start args. */
		struct FThreadStartArgs 
		{
			RThreadStart					mFunc;
			void*							mFuncArgs;
		};

		RThread() : _mThread() {}

		RThread(RThreadStart start, void* args, size_t coreAffinity = -1)
		{
			_mArgs.mFunc = start;
			_mArgs.mFuncArgs = args;

			_mThread.Create(0, RThread::StartThread, (void*)&_mArgs, coreAffinity);
		}

		/*! Fills this object with data of the current thread. */
		inline void FromCurrentThread() { _mThread.FromCurrentThread(); }

		/*! Sets core affinity (already masked, core 0 = 0, core 1 = 1, etc.) */
		inline void SetAffinity(size_t affinity) { _mThread.SetAffinity(affinity); }

		/*! Join the thread.*/
		inline void Join() { _mThread.Join(); }

		/*! Checks if the currently executing thread is equal to this thread. */
		inline bool IsCurrentThread() const { return _mThread.IsCurrentThread(); }

	private:
		static uint32_t StartThread(void* args)
		{
			FThreadStartArgs* arg = (FThreadStartArgs*)args;
			arg->mFunc(arg->mFuncArgs);

			return 0;
		}

	protected:
		FThreadStartArgs _mArgs;
		RPlatformThread _mThread;
	};
}