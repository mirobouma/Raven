#pragma once
#include <cinttypes>
#include "os/api/monitor.h"
#include "os/api/native_thread.h"
#include "util/path.h"
#include "util/result.h"

namespace Raven
{
    class INativeOS {
    protected:
        friend class Application;
    public:
        INativeOS() = default;
        virtual ~INativeOS() = default;

        INativeOS(const INativeOS&) = delete;
        INativeOS& operator= (const INativeOS&) = delete;

        virtual const char* get_name() const = 0;
        virtual Result initialize_core() = 0;
        virtual Result initialize() = 0;

        virtual void update() = 0;

        virtual bool supports_multiple_windows() const = 0;
        virtual bool supports_multithreading() const = 0;

        virtual int32_t get_monitor_count() = 0;
        virtual MonitorDescription get_main_monitor() = 0;
        virtual MonitorDescription get_monitor_description(int32_t index) = 0;

        virtual Path get_working_directory() const = 0;

        virtual bool has_environment_variable(const std::string& key) const = 0;
        virtual std::string get_environment_variable(const std::string& key) const = 0;
        virtual bool set_environment_variable(const std::string& key, const std::string& value) const = 0;

        virtual uint64_t get_unix_time() const = 0;
        virtual uint64_t get_system_time() const = 0;
        virtual uint64_t get_page_size() const = 0;

        virtual int get_exit_code(Result result) const = 0;
    };
}