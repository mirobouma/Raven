#include "windows/windows_thread.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace Raven
{
    ThreadID WindowsThread::get_current_thread_id_windows()
    {
        return ::GetCurrentThreadId();
    }

    Thread* WindowsThread::create_thread_windows()
    {
        return new WindowsThread();
    }

    void WindowsThread::make_current()
    {
        create_thread_func = create_thread_windows;
        get_current_thread_id_func = get_current_thread_id_windows;
    }
}