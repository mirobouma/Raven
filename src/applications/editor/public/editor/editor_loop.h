#pragma once
#include "core/main/application_loop.h"
#include "os/native_window.h"

namespace Raven
{
    class EditorLoop : public ApplicationLoop {
    public:
        virtual Result initialize(bool isStandalone) override;
        virtual void update(float deltaTime) override;
        virtual Result cleanup() override;

    private:
        INativeWindow* _mainWindow;
    };
}