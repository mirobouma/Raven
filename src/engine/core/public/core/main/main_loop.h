#pragma once
#include "core/errors.h"

class MainLoop {
public:
    MainLoop() = default;
    virtual ~MainLoop() = default;

    virtual Error initialize(bool isStandalone) = 0;
    virtual void update(float deltaTime) = 0;
    virtual Error cleanup() = 0;

protected:
    bool _isStandalone;
};