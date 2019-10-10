#pragma once
#include "core/os/os.h"
#include "core/errors.h"
#include "core/os/window_system.h"

class Engine {
public:
    Engine();
    ~Engine() = default;

    static inline Engine& get() { return *_singleton; }

    Error setup();
    Error update();
    Error cleanup();

    Engine(const Engine&) = delete;
    Engine& operator= (const Engine&) = delete;

    inline bool should_shutdown() { return _shouldShutdown; }

private:
    static Engine* _singleton;
    WindowSystem* _windowSystem;
    bool _shouldShutdown;
};