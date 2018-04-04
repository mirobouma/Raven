#pragma once

#include "Platform/Interfaces/IPlatformThreading.h"
#include <functional>
#include <process.h>
#include <atomic>
#include "coro.h"

#ifdef PLATFORM_LINUX

#include <pthread.h>
#include <unistd.h>


namespace Raven
{
	/*! \struct FPlatformLinuxThreadHandle
	\brief structure describing a platform thread handle.
	*/
	struct FPlatformLinuxThreadHandle : public IPlatformThreadHandle {

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

																					/*! Create the thread. */
		virtual bool Create(uint32_t stackSize, RThreadStart* start, void *arg, size_t coreAffinity = -1)
		{
			pthread_attr_t threadAttr;
			pthread_attr_init(&threadAttr);

			// Set stack size
			if (stackSize != 0)
			{
				pthread_attr_setstacksize(&threadAttr, stackSize);
			}

			#if defined(PLATFORM_LINUX)
			if (coreAffinity != -1)
			{
				SetAffinity(coreAffinity);
			}
			#endif

			int success = pthread_create(returnThread, &threadAttr, startRoutine, arg);

			// Cleanup
			pthread_attr_destroy(&threadAttr);

			return success == 0;
		}

		/*! Join the thread. */
		virtual void Join()
		{
			pthread_join(thread, nullptr);
		}

		/*! Set thread affinity. */
		virtual void SetAffinity(size_t affinity)
		{
			// Set core affinity
			cpu_set_t cpuSet;
			CPU_ZERO(&cpuSet);
			CPU_SET(coreAffinity, &cpuSet);
			pthread_attr_setaffinity_np(&threadAttr, sizeof(cpu_set_t), &cpuSet);
		}

		/*! Get platform-specific handle for this thread. */
		virtual IPlatformThreadHandle* GetHandle() { return &mHandle; };

	protected:
		FPlatformLinuxThreadHandle mHandle;
	};

	typedef RPlatformLinuxThread RPlatformThread;


	struct FLinuxFiberInitArgs {
		void* mArgs;
		RPlatformLinuxFiber* mFiber;
		RFiberStart mStart;
	};

	/*! \class RPlatformLinuxFiber
	\brief linux implementation for platform implementation of a fiber.
	*/
	class RPlatformLinuxFiber : public IPlatformFiber
	{
	public:
		RPlatformLinuxFiber(std::size_t stackSize, RFiberStart startRoutine, void *arg)
		{

			if (!startRoutine)
				return;

			coro_stack_alloc(&mStack, stackSize);
			mFiber = coro_create(stackSize, startRoutine, arg, mStack, stackSize);
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
			coro_transfer(mFiber, ((RPlatformWindowsFiber*)fiber)->mFiber);
		}

		virtual void Reset(RFiberStart startRoutine, void *arg)
		{
			size_t stackSize = mStack.ssze;

			coro_stack_free(mStack);
			coro_destroy(mFiber);

			coro_stack_alloc(&mStack, stackSize);
			mFiber = coro_create(stackSize, startRoutine, arg, mStack, stackSize);
		}

	protected:
		coro_context*		mFiber;
		coro_stack*			mStack;
	};

	typedef RPlatformLinuxFiber RPlatformFiber;
}

#endif