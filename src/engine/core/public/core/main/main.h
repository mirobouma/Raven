#pragma once
#include "core/os/os.h"
#include "core/errors.h"

class Main {
public:
    Main();
    ~Main() = default;

    static inline Main& get() { return *_singleton; }

    Error setup(int argc, char** argv);
    Error start();
    Error cleanup();

    Main(const Main&) = delete;
    Main& operator= (const Main&) = delete;

private:
    static Main* _singleton;
};