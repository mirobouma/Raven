#pragma once
#include "hal/logger.h"
#include <deque>

namespace Raven
{
	class FConsoleLogger : public ILogger
	{
	public:
		FConsoleLogger() : ILogger() {}

		// Inherited via ILogger
		virtual void Log(ELogSeverity severity, std::string message, va_list args) override;
		virtual ELogSeverity GetMinLogSeverity() const override;

		static const uint32_t maxMessages = 100;
		std::deque<std::string> mMessages;
	};
}