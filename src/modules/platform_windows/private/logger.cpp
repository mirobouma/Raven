#include "platform_windows/logger.h"
#include <stdarg.h>

namespace Raven 
{
	RPlatformWindowsLogger::RPlatformWindowsLogger(ELogSeverity minSeverity)
	{
		mMinSeverity = minSeverity;
	}

	void RPlatformWindowsLogger::Log(ELogSeverity severity, std::string message, va_list args)
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

		vprintf((prefix + message + "\n").c_str(), args);

	}

	ELogSeverity RPlatformWindowsLogger::GetMinLogSeverity() const
	{
		return mMinSeverity;
	}
}