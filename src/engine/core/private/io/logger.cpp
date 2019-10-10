#include "core/io/logger.h"
#include "core/os/os.h"

LogManager* LogManager::_singleton = nullptr;

Error LogManager::setup()
{
    _singleton = this;
    return Error::SUCCESS;
}

Error LogManager::cleanup()
{
    for (int i = 0; i < _loggers.size(); i++)
    {
        delete _loggers[i];
    }

    return Error::SUCCESS;
}

void LogManager::log(LogSeverity severity, String message, ...)
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

void ConsoleLogger::log(LogSeverity severity, String message, va_list args)
{
    // Unimplemented.
}

LogSeverity ConsoleLogger::get_min_log_severity() const
{
    return LogSeverity::LS_Debug;
}