#include "core/os/window_system.h"

WindowSystem* WindowSystem::_singleton = nullptr;

WindowSystem::WindowSystem() : _windows(), _eventListeners()
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
    return _windows.size();
}

bool WindowSystem::register_event_listener(IWindowEventListener* listener)
{
    _eventListeners.push_back(listener);
}

bool WindowSystem::unregister_event_listener(IWindowEventListener* listener)
{
    _eventListeners.erase(listener);
}