#include "PlatformLinux/PlatformLinuxLogger.h"
#include <stdarg.h>

namespace Raven
{
	RPlatformLinuxLogger::RPlatformLinuxLogger(ELogSeverity minSeverity)
	{
		mMinSeverity = minSeverity;
	}

	void RPlatformLinuxLogger::Log(ELogSeverity severity, std::string message, va_list args)
	{
		// If the severity
		if (severity < mMinSeverity)
		{
			return;
		}

		std::string prefix = "[Unknown] ";

		switch (severity)
		{
		case LS_Debug:
		{
			prefix = "[Debug] ";
		}
		break;
		case LS_Info:
		{
			prefix = "[Info] ";
		}
		break;
		case LS_Warning:
		{
			prefix = "[Warning] ";
		}
		break;
		case LS_Error:
		{
			prefix = "[Error] ";
		}
		break;
		}

		std::string msg = (prefix + message + std::string("\n"));
		vprintf(msg.c_str(), args);

	}

	ELogSeverity RPlatformLinuxLogger::GetMinLogSeverity() const
	{
		return mMinSeverity;
	}
}