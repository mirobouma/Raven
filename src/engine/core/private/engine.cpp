#include "core/engine.h"

Engine* Engine::_singleton = nullptr;

Engine::Engine()
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
    return Error::SUCCESS;
}