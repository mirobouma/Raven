#pragma once
#include "core/os/os.h"

class Main {
public:
    Main() = default;
    ~Main() = default;

    static Main& get() { return *_singleton; }
    void set_os_instance(OS* os);

    Main(const Main&) = delete;
    Main& operator= (const Main&) = delete;

private:
    static Main* _singleton;
    OS* _OS;
};