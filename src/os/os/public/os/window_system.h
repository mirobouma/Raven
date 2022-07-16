#pragma once
#include "os/native_window.h"
#include "os/window_event_listener.h"
#include <vector>

namespace Raven
{
    class WindowSystem {
    public:
        static inline WindowSystem& get() { return *_singleton; }

        void set_main_window(INativeWindow* window);
        INativeWindow* get_main_window();
        uint32_t get_window_count();

        void register_event_listener(IWindowEventListener* listener);
        void unregister_event_listener(IWindowEventListener* listener);

        INativeWindow* create_window(const std::string title, const Vector2Int size, WindowCreationFlags creationFlags);
        void destroy_window(INativeWindow* window);

    protected:
        friend class Engine;

        WindowSystem();
        WindowSystem(const WindowSystem&) = delete;
        WindowSystem& operator= (const WindowSystem&) = delete;
        virtual ~WindowSystem() = default;

        std::vector<INativeWindow*> _windows;
        std::vector<IWindowEventListener*> _eventListeners;
        INativeWindow* _mainWindow;

    private:
        static WindowSystem* _singleton;
    };
}
