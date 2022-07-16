#include "os/windows/windows_thread.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace Raven
{
	/*! \struct WindowsThreadHandle
\brief structure describing a platform thread handle.
*/
	class WindowsThreadHandle : public INativeThreadHandle 
	{
	public:
		HANDLE Handle;
		DWORD Id;
	};

	WindowsThread::WindowsThread()
	{
		mHandle = new WindowsThreadHandle();
	}

	WindowsThread::~WindowsThread()
	{
		delete mHandle;
	}
	
	INativeThread* WindowsThread::from_current_thread()
	{
		mHandle->Handle = GetCurrentThread();
		mHandle->Id = GetThreadId(mHandle->Handle);

		return this;
	}

	uint64_t WindowsThread::get_id()
	{
		return (uint64_t)mHandle->Id;
	}

	/*! Create the thread. */
	bool WindowsThread::create(uint32_t stackSize, NativeThreadStart* start, void* arg, size_t coreAffinity)
	{
		HANDLE handle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, stackSize, start, arg, CREATE_SUSPENDED, nullptr));

		if (handle == nullptr) {
			return false;
		}

		mHandle->Handle = handle;
		mHandle->Id = GetThreadId(handle);


		if (coreAffinity != -1)
		{
			// Set affinity on the thread.
			set_affinity(coreAffinity);
		}

		ResumeThread(mHandle->Handle);

		return true;
	}

	/*! Join the thread. */
	void WindowsThread::join()
	{
		WaitForSingleObject(mHandle->Handle, INFINITE);
	}

	/*! Set thread affinity. */
	void WindowsThread::set_affinity(size_t affinity)
	{
		DWORD_PTR mask = 1ull << affinity;
		SetThreadAffinityMask(mHandle->Handle, mask);
	}

	bool WindowsThread::is_current_thread() const
	{
		DWORD threadId = GetCurrentThreadId();
		return threadId == mHandle->Id;
	}

	/*! Get platform-specific handle for this thread. */
	INativeThreadHandle* WindowsThread::get_handle() 
	{ 
		return mHandle; 
	};
}