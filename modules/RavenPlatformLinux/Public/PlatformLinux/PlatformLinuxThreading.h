#pragma once

#include "Platform/Interfaces/IPlatformThreading.h"
#include <functional>
#include <atomic>
#include "coro.h"

#ifdef PLATFORM_LINUX

#include <pthread.h>
#include <unistd.h>


namespace Raven
{
	class RPlatformLinuxFiber;
	struct FLinuxFiberInitArgs {
		void* mArgs;
		RPlatformLinuxFiber* mFiber;
		RPlatformThreadStart mStart;
	};

	/*! Entry point typdef. */
	typedef void* (*RLinuxThreadStart)(void *arg);

	/*! \class RPlatformLinuxFiber
	\brief linux implementation for platform implementation of a fiber.
	*/
	class RPlatformLinuxFiber : public IPlatformFiber
	{
	public:
		RPlatformLinuxFiber() : mStack(nullptr), mFiber(nullptr) {}
		RPlatformLinuxFiber(std::size_t stackSize, RFiberStart startRoutine, void *arg)
		{

			if (!startRoutine)
				return;

			coro_stack_alloc(mStack, stackSize);
			coro_create(mFiber, startRoutine, arg, mStack, stackSize);
		}

		~RPlatformLinuxFiber()
		{
			coro_stack_free(mStack);
		}

		/*! Delete copy constructor because it doesn't make sense to copy a fiber. */
		RPlatformLinuxFiber(const RPlatformLinuxFiber &other) = delete;


		/*! Switch to fiber. */
		virtual void SwitchToFiber(IPlatformFiber* fiber)
		{
			//todo if mFiber is nullptr, convert thread into fiber.

			coro_transfer(mFiber, ((RPlatformLinuxFiber*)fiber)->mFiber);
		}

		virtual void Reset(RFiberStart startRoutine, void *arg)
		{
			size_t stackSize = mStack->ssze;

			coro_stack_free(mStack);
			coro_destroy(mFiber);

			coro_stack_alloc(mStack, stackSize);
			coro_create(mFiber, startRoutine, arg, mStack, stackSize);
		}

	protected:
		coro_context*		mFiber;
		coro_stack*			mStack;
	};

	typedef RPlatformLinuxFiber RPlatformFiber;

	/*! \struct FPlatformLinuxThreadHandle
	\brief structure describing a platform thread handle.
	*/
	struct FPlatformLinuxThreadHandle : public IPlatformThreadHandle {
		pthread_t mThread;
	};

	struct FLinuxThreadStartArgs {
		void* mArgs;
		RPlatformThreadStart mStart;
	};

	/*! \class RPlatformLinuxThread
	\brief Linux implementation of a thread.
	*/
	class RPlatformLinuxThread : public IPlatformThread
	{
	public:
		RPlatformLinuxThread() = default;
		RPlatformLinuxThread(const RPlatformLinuxThread&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a thread. */
		RPlatformLinuxThread& operator= (const RPlatformLinuxThread&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite a thread. */

		virtual void FromCurrentThread()
		{
			mHandle.mThread = pthread_self();
		}

		/*! Create the thread. */
		virtual bool Create(uint32_t stackSize, RPlatformThreadStart* start, void *arg, size_t coreAffinity = -1)
		{
			pthread_attr_t threadAttr;
			pthread_attr_init(&threadAttr);

			// Set stack size
			if (stackSize != 0)
			{
				pthread_attr_setstacksize(&threadAttr, stackSize);
			}

			mArgs.mArgs = arg;
			mArgs.mStart = start;

			int success = pthread_create(&mHandle.mThread, &threadAttr, LinuxThreadStart, mArgs);

			if (coreAffinity != -1)
			{
				SetAffinity(coreAffinity);
			}

			// Cleanup
			pthread_attr_destroy(&threadAttr);

			return success == 0;
		}

		/*! Join the thread. */
		virtual void Join()
		{
			pthread_join(mHandle.mThread, nullptr);
		}

		/*! Set thread affinity. */
		virtual void SetAffinity(size_t affinity)
		{
			// Set core affinity
			cpu_set_t cpuSet;
			CPU_ZERO(&cpuSet);
			CPU_SET(1 << affinity, &cpuSet);

			pthread_attr_t attr;
			pthread_getattr_np(mHandle.mThread, &attr);
			pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuSet);
		}

		/*! Get platform-specific handle for this thread. */
		virtual IPlatformThreadHandle* GetHandle() { return &mHandle; };

		/*! Check if this thread equals the currently executing thread. */
		virtual bool IsCurrentThread() const
		{
			pthread_t currentThread = pthread_self();
			return pthread_equal(threadId == mHandle.mThread);
		}

	private:
		static void* LinuxThreadStart(void* arg)
		{
			FLinuxThreadStartArgs* args = (FLinuxThreadStartArgs*)arg;
			
			args->mStart(args->mArgs);

			return 0;
		}

	protected:
		FLinuxThreadStartArgs	   mArgs;
		FPlatformLinuxThreadHandle mHandle;
	};

	typedef RPlatformLinuxThread RPlatformThread;
}

#endif