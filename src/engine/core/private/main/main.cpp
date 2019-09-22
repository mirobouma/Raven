#include "core/main/main.h"
#include "core/main/main_loop.h"
#include "core/engine.h"

Main* Main::_singleton = nullptr;

Main::Main()
{
    _singleton = this;
}

Error Main::setup(int argc, char** argv)
{
    OS& os = OS::get();
    Engine* engine = new Engine();

    parse_args(argc, argv);

    // Initialize core systems required to initialize before anything else.
    Error result = os.initialize_core();
    if (result != Error::SUCCESS)
        return result;

    // Initialize engine.
    result = engine->setup();
    if (result != Error::SUCCESS)
        return result;

    // Initialize OS specific systems.
    result = os.initialize();
    if (result != Error::SUCCESS)
        return result;

    return Error::SUCCESS;
}

Error Main::parse_args(int argc, char** argv)
{
    // Parse command-line args.
    return Error::SUCCESS;
}

Error Main::run()
{
    OS& os = OS::get();

    if (!_main_loop)
        return;
    
    // The root main loop is always a standalone.
    // If the game is an editor, the dynamic game library will not
    // be a standalone, and it's viewport creation is handled by
    // the editor loop.
    Error result = _main_loop->initialize(true);
    if (result != Error::SUCCESS)
    {
        _main_loop->cleanup();
        return result;
    }
    
    while (!_engine->should_shutdown()) 
    {
        // Update OS messages.
        os.update();

        // Update the main loop.
        result = update();

        if (result != Error::SUCCESS)
            break;          
    }

    if (result != Error::SUCCESS)
    {
        _main_loop->cleanup();
        return result;
    }

    result = _main_loop->cleanup();
    return result;
}

Error Main::cleanup()
{
    _engine->cleanup();
    delete _engine;

    return Error::SUCCESS;
}