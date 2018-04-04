#pragma once

#include "PlatformWindows/PlatformWindows.h"
#include "Platform/Interfaces/IPlatformThreading.h"
#include <functional>
#include <process.h>
#include <atomic>

namespace Raven
{
	/*! \struct FPlatformWindowsThreadHandle
	\brief structure describing a platform thread handle.
	*/
	struct FPlatformWindowsThreadHandle : public IPlatformThreadHandle {
		HANDLE Handle;
		DWORD Id;
	};

	class RPlatformWindowsFiber;

	struct FWinFiberInitArgs {
		void* mArgs;
		RPlatformWindowsFiber* mFiber;
		RFiberStart mStart;
	};

	/*! \class RPlatformWindowsFiber
	\brief Interface for platform implementation of a fiber.
	*/
	class RPlatformWindowsFiber : public IPlatformFiber
	{
	public:
		RPlatformWindowsFiber(std::size_t stackSize, RFiberStart startRoutine, void *arg)
		{
			mFiber = 0;
			mArgs.mArgs = arg;
			mArgs.mFiber = this;
			mArgs.mStart = startRoutine;
			mStackSize = stackSize;

			if (!startRoutine)
				return;

			mFiber = CreateFiber(stackSize, InitFiber, &mArgs);
		}

		/*! Delete copy constructor because it doesn't make sense to copy a fiber. */
		RPlatformWindowsFiber(const RPlatformWindowsFiber &other) = delete;

		/*! Switch to fiber. */
		virtual void SwitchToFiber(IPlatformFiber* fiber)
		{
			if (mFiber == 0)
			{
				mFiber = ConvertThreadToFiber(0);
			}

			::SwitchToFiber(((RPlatformWindowsFiber*)fiber)->mFiber);
		}

		/*! Initialize the fiber and start. */
		static void InitFiber(void* arg)
		{
			FWinFiberInitArgs* args = (FWinFiberInitArgs*)arg;
			args->mStart(args->mArgs);
		}

		virtual void Reset(RFiberStart startRoutine, void *arg)
		{
			mArgs.mArgs = arg;
			mArgs.mStart = startRoutine;
			mFiber = CreateFiber(mStackSize, InitFiber, &mArgs);
		}

	protected:
		void* mFiber;
		FWinFiberInitArgs mArgs;
		size_t mStackSize;
	};

	typedef RPlatformWindowsFiber RPlatformFiber;

	/*! \class RPlatformWindowsThread
	\brief Windows implementation of a thread.
	*/
	class RPlatformWindowsThread : public IPlatformThread
	{
	public:
		RPlatformWindowsThread() = default;
		RPlatformWindowsThread(const RPlatformWindowsThread&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a thread. */
		RPlatformWindowsThread& operator= (const RPlatformWindowsThread&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a thread. */

		/*! Create the thread. */
		virtual bool Create(uint32_t stackSize, RThreadStart* start, void *arg, size_t coreAffinity = -1)
		{
			HANDLE handle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, stackSize, start, arg, CREATE_SUSPENDED, nullptr));

			if (handle == nullptr) {
				return false;
			}

			if (coreAffinity != -1)
			{
				// Set affinity on the thread.
				SetAffinity(coreAffinity);
			}

			mHandle.Handle = handle;
			mHandle.Id = GetThreadId(handle);
			ResumeThread(mHandle.Handle);

			return true;
		}

		/*! Join the thread. */
		virtual void Join()
		{
			WaitForSingleObject(mHandle.Handle, INFINITE);
		}

		/*! Set thread affinity. */
		virtual void SetAffinity(size_t affinity)
		{
			DWORD_PTR mask = 1ull << affinity;
			SetThreadAffinityMask(mHandle.Handle, mask);
		}

		/*! Get platform-specific handle for this thread. */
		virtual IPlatformThreadHandle* GetHandle() { return &mHandle; };

	protected:
		FPlatformWindowsThreadHandle mHandle;
	};

	typedef RPlatformWindowsThread RPlatformThread;

}