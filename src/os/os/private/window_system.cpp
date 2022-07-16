#include "os/window_system.h"
#include "util/logging_macros.h"

namespace Raven
{
    WindowSystem* WindowSystem::_singleton = nullptr;

    WindowSystem::WindowSystem() : _windows(), _eventListeners(), _mainWindow(nullptr)
    {
        _singleton = this;
    }

    void WindowSystem::set_main_window(INativeWindow* window)
    {
        _mainWindow = window;
    }

    INativeWindow* WindowSystem::get_main_window()
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

    INativeWindow* WindowSystem::create_window(const std::string title, const Vector2Int size, WindowCreationFlags creationFlags = WindowCreationFlags_Shown)
    {
        INativeWindow* window = new NativeWindow(title, size, creationFlags);
        RASSERT(window != nullptr);

        _windows.push_back(window);

        if (_mainWindow == nullptr)
            _mainWindow = window;

        return window;
    }

    void WindowSystem::destroy_window(INativeWindow* window)
    {
        if (_mainWindow == window)
            _mainWindow = nullptr;

        _windows.erase(std::remove(_windows.begin(), _windows.end(), window), _windows.end());
        delete window;
    }
}
