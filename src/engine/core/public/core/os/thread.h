#pragma once
#include <cinttypes>

typedef uint64_t ThreadID;

class Thread {
protected:
    static Thread* (*create_thread_func)();
    static ThreadID (*get_current_thread_id_func)();

public:
    static ThreadID get_current_thread_id();
};