#pragma once
#include "core/os/dynamic_library.h"
#include "core/os/os.h"

#include "core/main/application_loop.h"
#include "core/errors.h"

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

        Error setup(int argc, char** argv);
        Error run();
        Error update();
        Error cleanup();

        Error parse_args(int argc, char** argv);

        Application(const Application&) = delete;
        Application& operator= (const Application&) = delete;

    private:
        static Application* _singleton;
        ApplicationLoop* _application_loop;
    };
}