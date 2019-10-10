#pragma once
#include "core/os/os.h"

class OSWindows : public OS
{
public:
    OSWindows() : OS() {}

    virtual String get_name() const { return "Windows"; }
    virtual Error initialize_core() override;
    virtual Error initialize() override;

    virtual void update() override;

    virtual inline bool supports_multiple_windows() const override { return true; }
    virtual inline bool supports_multithreading() const override { return true; }

    virtual int32_t get_monitor_count() override;
    virtual MonitorDescription get_main_monitor() override;
    virtual MonitorDescription get_monitor_description(int32_t index) override;

    virtual Path get_working_directory() const override;

    virtual bool has_environment_variable(const String& key) const;
    virtual String get_environment_variable(const String& key) const;
    virtual bool set_environment_variable(const String& key, const String& value) const;

    virtual uint64_t get_unix_time() const;
    virtual uint64_t get_system_time() const;
    virtual uint64_t get_page_size() const;

    virtual int get_exit_code(Error error) const override;
};