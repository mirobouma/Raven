#pragma once
#include "core/main/application_loop.h"
#include "core/os/window.h"

namespace Raven
{
    class EditorLoop : public ApplicationLoop {
    public:
        virtual Error initialize(bool isStandalone) override;
        virtual void update(float deltaTime) override;
        virtual Error cleanup() override;

    private:
        Window* _mainWindow;
    };
}