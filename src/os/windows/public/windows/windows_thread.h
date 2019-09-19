#pragma once
#include "core/os/thread.h"

class WindowsThread : Thread {
    static void make_current();
};