#include "core/os/thread.h"

namespace Raven
{
    Thread* (*Thread::create_thread_func)() = nullptr;
    ThreadID(*Thread::get_current_thread_id_func)() = nullptr;

    ThreadID Thread::get_current_thread_id()
    {
        if (get_current_thread_id_func)
        {
            return get_current_thread_id_func();
        }

        return 0;
    }
}
