#pragma once
#include "core/vector.h"
#include "core/os/window.h"
#include "core/window_event_listener.h"

class WindowSystem 
{
public:
    static inline WindowSystem& get() { return *_singleton; }

    void set_main_window(Window* window);
    Window* get_main_window();
    uint32_t get_window_count();

    bool register_event_listener();
    bool unregister_event_listener();

    virtual Window* create_window() = 0;

protected:
    WindowSystem() = default;
    WindowSystem(const WindowSystem&) = delete;
    WindowSystem& operator= (const WindowSystem&) = delete;
    virtual ~WindowSystem() = default;

    Vector<Window*> _windows;
    Vector<IWindowEventListener*> _eventListeners;
    Window* _mainWindow;

private:
    static WindowSystem* _singleton;
};

