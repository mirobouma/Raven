#include "core/engine.h"

Engine* Engine::_singleton = nullptr;

Engine::Engine() : _shouldShutdown(false)
{
    _singleton = this;
}

Error Engine::setup()
{
    return Error::SUCCESS;
}

Error Engine::update()
{
    return Error::SUCCESS;
}

Error Engine::cleanup()
{
    delete _singleton;
    _singleton = nullptr;

    return Error::SUCCESS;
}