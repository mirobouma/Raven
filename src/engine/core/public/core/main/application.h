#pragma once
#include "os/os.h"
#include "core/main/application_loop.h"
#include "util/result.h"

#include <cinttypes>

#define RAVEN_APPLICATION(T) \
Raven::ApplicationLoop* raven_create_application() { return new T(); }

namespace Raven
{
    class ApplicationLoop;
    class Engine;
    class Application {
    public:
        Application();
        ~Application() = default;

        static inline Application& get() { return *_singleton; }

        Result setup(int argc, char** argv);
        Result run();
        Result update();
        Result cleanup();

        Result parse_args(int argc, char** argv);

        Application(const Application&) = delete;
        Application& operator= (const Application&) = delete;

    private:
        static Application* _singleton;
        ApplicationLoop* _application_loop;
    };
}