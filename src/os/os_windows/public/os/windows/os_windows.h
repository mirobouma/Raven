#pragma once
#include "os/api/native_os.h"

namespace Raven
{
    class OSWindows : public INativeOS
    {
    public:
        static inline OSWindows& get() { return *_singleton; }

        OSWindows() : INativeOS() { _singleton = this; }
        
        virtual const char* get_name() const { return "Windows"; }
        virtual Result initialize_core() override;
        virtual Result initialize() override;

        virtual void update() override;

        virtual inline bool supports_multiple_windows() const override { return true; }
        virtual inline bool supports_multithreading() const override { return true; }

        virtual int32_t get_monitor_count() override;
        virtual MonitorDescription get_main_monitor() override;
        virtual MonitorDescription get_monitor_description(int32_t index) override;

        virtual Path get_working_directory() const override;

        virtual bool has_environment_variable(const std::string& key) const;
        virtual std::string get_environment_variable(const std::string& key) const;
        virtual bool set_environment_variable(const std::string& key, const std::string& value) const;

        virtual uint64_t get_unix_time() const;
        virtual uint64_t get_system_time() const;
        virtual uint64_t get_page_size() const;

        virtual int get_exit_code(Result error) const override;
    private:
        static OSWindows* _singleton;
    };

    typedef OSWindows OS;
}