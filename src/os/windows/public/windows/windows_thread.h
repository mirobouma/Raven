#pragma once
#include "core/os/thread.h"

class WindowsThread : Thread {
private:
    static ThreadID get_current_thread_id_windows();
    static Thread* create_thread_windows();

protected:
    friend class OSWindows;
    static void make_current();
};