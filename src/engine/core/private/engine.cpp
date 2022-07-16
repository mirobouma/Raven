#include "core/engine.h"

namespace Raven
{
    Engine* Engine::_singleton = nullptr;

    Engine::Engine() : _shouldShutdown(false)
    {
        _singleton = this;
    }

    Result Engine::setup()
    {
        _windowSystem = new WindowSystem();
        return Result::SUCCESS;
    }

    Result Engine::update()
    {
        return Result::SUCCESS;
    }

    Result Engine::cleanup()
    {
        delete _windowSystem;
        _windowSystem = nullptr;

        delete _singleton;
        _singleton = nullptr;

        return Result::SUCCESS;
    }
}