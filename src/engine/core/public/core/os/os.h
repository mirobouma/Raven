#pragma once
#include <cinttypes>
#include "core/os/monitor.h"
#include "core/os/thread.h"
#include "core/string.h"
#include "core/path.h"
#include "core/errors.h"
#include "core/os/window_system.h"

class OS {
protected:
    friend class Main;
    static void set_singleton(OS* os) { _singleton = os; }
    static OS* _singleton;

public:
    OS();
    virtual ~OS() = default;

    OS(const OS&) = delete;
    OS& operator= (const OS&) = delete;

    inline static OS& get() { return *_singleton; } 

    virtual String get_name() const = 0;
    virtual Error initialize_core() = 0;
    virtual Error initialize() = 0;

    virtual void update() = 0;

    virtual bool supports_multiple_windows() const = 0;
    virtual bool supports_multithreading() const = 0;

    virtual int32_t get_monitor_count() = 0;
    virtual MonitorDescription get_main_monitor() = 0;
    virtual MonitorDescription get_monitor_description(int32_t index) = 0;

    virtual Path get_working_directory() const = 0;

    virtual bool has_environment_variable(const String& key) const = 0;
    virtual String get_environment_variable(const String& key) const = 0;
    virtual bool set_environment_variable(const String& key, const String& value) const = 0;

    virtual uint64_t get_unix_time() const = 0;
    virtual uint64_t get_system_time() const = 0;

    virtual int get_exit_code(Error error) const = 0; 
};
