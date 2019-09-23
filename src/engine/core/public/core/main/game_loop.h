#pragma once
#include "core/main/main_loop.h"

class GameLoop : public MainLoop
{
    virtual Error initialize(bool isStandalone);
    virtual void update(float deltaTime);
    virtual Error cleanup();
};