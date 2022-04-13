#pragma once
#include "core/os/window_system.h"
#include "math/vector.h"

namespace Raven
{
    class Window;
    class IWindowEventListener {
    public:
        virtual void on_window_resize(Window* window, Vector2 size) = 0;
        virtual void on_window_close(Window* window) = 0;
        virtual void on_window_minimize(Window* window) = 0;
        virtual void on_window_focus(Window* window) = 0;
        virtual void on_window_focus_lost(Window* window) = 0;
        virtual void on_window_move(Window* window, Vector2 position) = 0;
    };
}
