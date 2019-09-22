#include "windows/os_windows.h"
#include "core/main/main.h"
#include "math/math.h"

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

    Error result = main->setup(argc, argv);
    if (result != Error::SUCCESS)
        return os.get_exit_code(result);

    result = main->run();
    if (result != Error::SUCCESS)
        return os.get_exit_code(result);

    main->cleanup();

    delete main;
    return os.get_exit_code(result);
}