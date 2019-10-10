#pragma once
#include "core/vector.h"
#include "core/os/window.h"
#include "core/os/window_event_listener.h"

class WindowSystem {
public:
    static inline WindowSystem& get() { return *_singleton; }

    void set_main_window(Window* window);
    Window* get_main_window();
    uint32_t get_window_count();

    void register_event_listener(IWindowEventListener* listener);
    void unregister_event_listener(IWindowEventListener* listener);

    Window* create_window(const String title, const Vector2Int size, WindowCreationFlags creationFlags);
    void destroy_window(Window* window);

protected:
    friend class Engine;

    WindowSystem();
    WindowSystem(const WindowSystem&) = delete;
    WindowSystem& operator= (const WindowSystem&) = delete;
    virtual ~WindowSystem() = default;

    Vector<Window*> _windows;
    Vector<IWindowEventListener*> _eventListeners;
    Window* _mainWindow;

private:
    static WindowSystem* _singleton;
};

