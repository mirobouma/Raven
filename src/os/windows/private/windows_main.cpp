#include "windows/os_windows.h"
#include "core/main/main.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char** argv)
{
    OSWindows os;
    Main* main = new Main();

    if (main->setup(argc, argv))
    {
        main->start();
    }

    main->cleanup();

    delete main;
    return os.get_exit_code();
}