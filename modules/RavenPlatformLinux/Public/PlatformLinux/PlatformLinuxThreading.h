#pragma once

#include "Platform/Interfaces/IPlatformThreading.h"
#include <functional>
#include <atomic>
#include "boost_context/fcontext.h"
#include "Core/Memory/MemoryUtils.h"
#include "PlatformLinuxInfo.h"
#include "Core/Debug/LogManager.h"

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
        RPlatformLinuxFiber() : mFiber(nullptr), mStackSize(0), mStack(), mArg(nullptr) {}
        RPlatformLinuxFiber(std::size_t stackSize, RFiberStart startRoutine, void *arg)
        {
            if (!startRoutine)
                return;

            mArg = arg;

            if(stackSize == 0)
            {
                stackSize = 512000;  //16k
            }

            mStack = boost_context::create_fcontext_stack(Memory::AlignUp(stackSize, RPlatformLinuxInfo::GetPageSize()));
            mStackSize = mStack.ssize;
            mFiber = boost_context::make_fcontext(static_cast<char*>(mStack.sptr), mStack.ssize, startRoutine);
        }

        ~RPlatformLinuxFiber()
        {
            if(mStack.sptr != nullptr) {
                boost_context::destroy_fcontext_stack(&mStack);
            }
        }

        /*! Delete copy constructor because it doesn't make sense to copy a fiber. */
        RPlatformLinuxFiber(const RPlatformLinuxFiber &other) = delete;


        /*! Switch to fiber. */
        virtual void SwitchToFiber(IPlatformFiber* fiber) override
        {

            boost_context::jump_fcontext(&mFiber, ((RPlatformLinuxFiber*)fiber)->mFiber, ((RPlatformLinuxFiber*)fiber)->mArg);
        }

        virtual void Reset(RFiberStart startRoutine, void *arg) override
        {
            RASSERT(mStackSize != 0);

            mArg = arg;
            mFiber = boost_context::make_fcontext(static_cast<char*>(mStack.sptr), mStack.ssize, startRoutine);
        }

    protected:
        boost_context::fcontext_t		    mFiber;
        boost_context::fcontext_stack_t	    mStack;
        void*                               mArg;
        size_t                              mStackSize;

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
        RPlatformThreadStart* mStart;
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

		static uint64_t GetCurrentThreadID()
		{
			return RPlatformWindowsThread().FromCurrentThread()->GetID();
		}

        virtual IPlatformThread* FromCurrentThread()
        {
            mHandle.mThread = pthread_self();
			return this;
        }

		virtual uint64_t GetID()
		{
			return mId;
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

            int success = pthread_create(&mHandle.mThread, &threadAttr, LinuxThreadStart, &mArgs);

            if (coreAffinity != -1)
            {
                SetAffinity(coreAffinity);
            }

			pthread_id_np_t   tid;
			pthread_getunique_np(&mHandle.mThread, &tid);

			mId = (uint64_t)tid;

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
            pthread_setaffinity_np(mHandle.mThread, sizeof(cpu_set_t), &cpuSet);
        }

        /*! Get platform-specific handle for this thread. */
        virtual IPlatformThreadHandle* GetHandle() { return &mHandle; };

        /*! Check if this thread equals the currently executing thread. */
        virtual bool IsCurrentThread() const
        {
            pthread_t currentThread = pthread_self();
            return pthread_equal(currentThread, mHandle.mThread);
        }

    private:
        static void* LinuxThreadStart(void* arg)
        {
            FLinuxThreadStartArgs* args = (FLinuxThreadStartArgs*)arg;

            args->mStart(args->mArgs);

            return (void*)0;
        }

    protected:
        FLinuxThreadStartArgs	   mArgs;
		uint64_t				   mId;
        FPlatformLinuxThreadHandle mHandle;
    };

    typedef RPlatformLinuxThread RPlatformThread;
}

#endif