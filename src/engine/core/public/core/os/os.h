#pragma once
#include <cinttypes>
#include "core/os/monitor.h"
#include "core/os/thread.h"
#include "core/string.h"

class OS {
protected:
    friend class Main;
    static void set_singleton(OS* os) { _singleton = os; }

    virtual bool initialize_core_systems() = 0;
    virtual bool initialize() = 0;

    virtual bool supports_multiple_windows() const = 0;
    virtual bool supports_multithreading() const = 0;

    virtual int32_t get_mounitor_count() = 0;
    virtual int32_t get_monitor(uint32_t index) = 0;
    virtual int get_current_monitor() = 0;
    virtual MonitorDescription get_monitor_description(uint32_t index) = 0;

    virtual String get_working_directory() const = 0;

    virtual bool has_environment_variable(const String& key) const = 0;
    virtual String get_environment_variable(const String& key) const = 0;
    virtual bool set_environment_variable(const String& key, const String& value) const = 0;

public:
    OS() = default;
    virtual ~OS() = default;

    OS(const OS&) = delete;
    OS& operator= (const OS&) = delete;

    inline static OS& get() { return *_singleton; } 

protected:
    static OS* _singleton;
};
