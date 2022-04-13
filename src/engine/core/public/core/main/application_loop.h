#pragma once
#include "core/errors.h"

namespace Raven
{
    class ApplicationLoop {
    public:
        ApplicationLoop() = default;
        virtual ~ApplicationLoop() = default;

        virtual Error initialize(bool isStandalone) = 0;
        virtual void update(float deltaTime) = 0;
        virtual Error cleanup() = 0;

    protected:
        bool _isStandalone;
    };
}