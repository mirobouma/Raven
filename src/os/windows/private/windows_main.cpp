#include "windows/os_windows.h"
#include "core/main/main.h"

int main(int argc, char** argv)
{
    OSWindows os = OSWindows();
    Main main = Main();

    Main::get().set_os_instance(&os);
    return 0;
}