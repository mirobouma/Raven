#include "core/main/main.h"

Main* Main::_singleton = nullptr;

Main::Main()
{
    _singleton = this;
}

Error Main::setup(int argc, char** argv)
{
    OS& os = OS::get();

    // Initialize core systems required to initialize before anything else.
    Error result = os.initialize_core();
    if (result != Error::SUCCESS)
        return result;

    // Initialize OS specific systems.
    result = os.initialize();
    if (result != Error::SUCCESS)
        return result;

    return Error::SUCCESS;
}

Error Main::start()
{
    return Error::SUCCESS;
}

Error Main::cleanup()
{
    return Error::SUCCESS;
}