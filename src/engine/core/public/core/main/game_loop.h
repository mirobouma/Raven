#pragma once
#include "core/main/application_loop.h"

namespace Raven
{
    class INativeWindow;
    class GameLoop : public ApplicationLoop
    {
    public:
        virtual Result initialize(bool isStandalone);
        virtual void update(float deltaTime);
        virtual Result cleanup();

    private:
        INativeWindow* _mainWindow;
    };
}