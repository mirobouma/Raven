#pragma once
#include "core/main/application_loop.h"

namespace Raven
{
    class Window;
    class GameLoop : public ApplicationLoop
    {
    public:
        virtual Error initialize(bool isStandalone);
        virtual void update(float deltaTime);
        virtual Error cleanup();

    private:
        Window* _mainWindow;
    };
}