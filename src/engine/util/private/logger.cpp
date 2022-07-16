#include "util/logger.h"

namespace Raven
{
    LogManager* LogManager::_singleton = nullptr;

    Result LogManager::setup()
    {
        _singleton = this;
        return Result::SUCCESS;
    }

    Result LogManager::cleanup()
    {
        for (int i = 0; i < _loggers.size(); i++)
        {
            delete _loggers[i];
        }

        return Result::SUCCESS;
    }

    void LogManager::log(LogSeverity severity, const char* message, ...)
    {
        va_list args;
        va_start(args, message);

        for (int i = 0; i < _loggers.size(); ++i)
        {
            _loggers[i]->log(severity, message, args);
        }

        va_end(args);
    }

    void LogManager::register_logger(Logger* logger)
    {
        _loggers.push_back(logger);
    }

    void LogManager::deregister_logger(Logger* logger)
    {
        _loggers.erase(std::remove(_loggers.begin(), _loggers.end(), logger));
    }

    void ConsoleLogger::log(LogSeverity severity, const char* message, va_list args)
    {
        // Unimplemented.
    }

    LogSeverity ConsoleLogger::get_min_log_severity() const
    {
        return LogSeverity::LS_Debug;
    }
}