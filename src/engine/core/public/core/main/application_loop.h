#pragma once
#include "util/result.h"

namespace Raven
{
    class ApplicationLoop {
    public:
        ApplicationLoop() = default;
        virtual ~ApplicationLoop() = default;

        virtual Result initialize(bool isStandalone) = 0;
        virtual void update(float deltaTime) = 0;
        virtual Result cleanup() = 0;

    protected:
        bool _isStandalone;
    };
}