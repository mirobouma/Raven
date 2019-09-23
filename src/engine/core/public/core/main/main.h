#pragma once
#include "core/os/os.h"
#include "core/errors.h"

class MainLoop;
class Engine;
class Main {
public:
    Main();
    ~Main() = default;

    static inline Main& get() { return *_singleton; }

    Error setup(int argc, char** argv);
    Error run();
    Error update();
    Error cleanup();

    Error parse_args(int argc, char** argv);

    Main(const Main&) = delete;
    Main& operator= (const Main&) = delete;

private:
    static Main* _singleton;
    MainLoop* _main_loop;
};