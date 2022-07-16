#pragma once
#include "os/window_system.h"
#include "math/vector.h"

namespace Raven
{
    class INativeWindow;
    class IWindowEventListener {
    public:
        virtual void on_window_resize(INativeWindow* window, Vector2 size) = 0;
        virtual void on_window_close(INativeWindow* window) = 0;
        virtual void on_window_minimize(INativeWindow* window) = 0;
        virtual void on_window_focus(INativeWindow* window) = 0;
        virtual void on_window_focus_lost(INativeWindow* window) = 0;
        virtual void on_window_move(INativeWindow* window, Vector2 position) = 0;
    };
}
