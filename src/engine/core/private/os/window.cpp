#include "core/os/window.h"

namespace Raven
{
    Window* (*Window::create_window_func)(const String, const Vector2Int, WindowCreationFlags) = nullptr;

    Window* Window::create(const String title, const Vector2Int size, WindowCreationFlags creationFlags = WindowCreationFlags_Shown)
    {
        if (create_window_func)
        {
            return create_window_func(title, size, creationFlags);
        }

        return nullptr;
    }
}