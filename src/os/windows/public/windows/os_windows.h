#pragma once
#include "core/os/os.h"

class OSWindows : OS
{
    virtual bool initialize_core_systems();
    virtual bool initialize();

    virtual inline bool supports_multiple_windows() final { return true; }
    virtual inline bool supports_multithreading() const final { return true; }

    virtual int32_t get_mounitor_count();
    virtual int32_t get_monitor(uint32_t index);
    virtual int get_current_monitor();
    virtual MonitorDescription get_monitor_description(uint32_t index);

    virtual String get_working_directory() const;
    virtual String get_environment_variable(const String& name) const;
};