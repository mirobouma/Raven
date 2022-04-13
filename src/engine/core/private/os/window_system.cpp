#include "core/os/window_system.h"
#include "core/logging_macros.h"

namespace Raven
{
    WindowSystem* WindowSystem::_singleton = nullptr;

    WindowSystem::WindowSystem() : _windows(), _eventListeners(), _mainWindow(nullptr)
    {
        _singleton = this;
    }

    void WindowSystem::set_main_window(Window* window)
    {
        _mainWindow = window;
    }

    Window* WindowSystem::get_main_window()
    {
        return _mainWindow;
    }

    uint32_t WindowSystem::get_window_count()
    {
        return (uint32_t)_windows.size();
    }

    void WindowSystem::register_event_listener(IWindowEventListener* listener)
    {
        _eventListeners.push_back(listener);
    }

    void WindowSystem::unregister_event_listener(IWindowEventListener* listener)
    {
        _eventListeners.erase(std::remove(_eventListeners.begin(), _eventListeners.end(), listener), _eventListeners.end());
    }

    Window* WindowSystem::create_window(const String title, const Vector2Int size, WindowCreationFlags creationFlags = WindowCreationFlags_Shown)
    {
        Window* window = Window::create(title, size, creationFlags);
        RASSERT(window != nullptr);

        _windows.push_back(window);

        if (_mainWindow == nullptr)
            _mainWindow = window;

        return window;
    }

    void WindowSystem::destroy_window(Window* window)
    {
        if (_mainWindow == window)
            _mainWindow = nullptr;

        _windows.erase(std::remove(_windows.begin(), _windows.end(), window), _windows.end());
        delete window;
    }
}
