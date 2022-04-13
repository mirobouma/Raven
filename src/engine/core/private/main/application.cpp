#include "core/main/application.h"
#include "core/main/application_loop.h"
#include "core/engine.h"
#include "core/io/logger.h"

Raven::ApplicationLoop* raven_create_application();

namespace Raven
{
    Application* Application::_singleton = nullptr;
  
    Application::Application()
    {
        _singleton = this;
    }

    Error Application::setup(int argc, char** argv)
    {
        LogManager* logManager = new LogManager();
        logManager->setup();

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

        _application_loop = raven_create_main_loop();
        return Error::SUCCESS;
    }

    Error Application::parse_args(int argc, char** argv)
    {
        // Parse command-line args.
        return Error::SUCCESS;
    }

    Error Application::run()
    {
        OS& os = OS::get();
        Engine& engine = Engine::get();

        if (!_application_loop)
            return Error::ERR_DOES_NOT_EXIST;

        // The root main loop is always a standalone.
        // If the game is an editor, the dynamic game library will not
        // be a standalone, and it's viewport creation is handled by
        // the editor loop.
        Error result = _application_loop->initialize(true);
        if (result != Error::SUCCESS)
        {
            _application_loop->cleanup();
            return result;
        }

        while (!engine.should_shutdown())
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
            _application_loop->cleanup();
            return result;
        }

        result = _application_loop->cleanup();
        return result;
    }

    Error Application::update()
    {
        float deltaTime = 0.016f;
        _application_loop->update(deltaTime);

        return Error::SUCCESS;
    }

    Error Application::cleanup()
    {
        Engine::get().cleanup();
        return Error::SUCCESS;
    }
}