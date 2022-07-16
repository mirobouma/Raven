#pragma once
#include "os/os.h"
#include "util/result.h"
#include "os/window_system.h"

namespace Raven
{
    class Engine {
    public:
        Engine();
        ~Engine() = default;

        static inline Engine& get() { return *_singleton; }

        Result setup();
        Result update();
        Result cleanup();

        Engine(const Engine&) = delete;
        Engine& operator= (const Engine&) = delete;

        inline bool should_shutdown() { return _shouldShutdown; }

    private:
        static Engine* _singleton;
        WindowSystem* _windowSystem;
        bool _shouldShutdown;
    };
}