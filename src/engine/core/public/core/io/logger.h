#pragma once
#include <stdarg.h>
#include "core/vector.h"
#include "core/string.h"
#include "core/errors.h"

namespace Raven
{
    typedef enum LogSeverity {
        LS_Debug = 0,
        LS_Info,
        LS_Warning,
        LS_Error,
    } LogSeverity;

    class Logger {
    public:
        Logger() = default;
        virtual ~Logger() = default;

        virtual void log(LogSeverity severity, String message, va_list args) = 0;
        virtual LogSeverity get_min_log_severity() const = 0;
    };

    class ConsoleLogger : public Logger {
    public:
        ConsoleLogger() = default;

        virtual void log(LogSeverity severity, String message, va_list args) override;
        virtual LogSeverity get_min_log_severity() const override;
    };

    class LogManager {
    protected:
        friend class Application;
        LogManager() = default;
        ~LogManager() = default;

        Error setup();
        Error cleanup();

        LogManager(const Application&) = delete;
        LogManager& operator= (const Application&) = delete;

    public:
        static inline LogManager& get() { return *_singleton; }

        void log(LogSeverity severity, String message, ...);

        void register_logger(Logger* logger);
        void deregister_logger(Logger* logger);

    private:
        static LogManager* _singleton;
        Vector<Logger*> _loggers;
    };
}