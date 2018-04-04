#include "Platform/Generic/GenericPlatformLogger.h"
#include <stdarg.h>

namespace Raven
{
	RGenericPlatformLogger::RGenericPlatformLogger(ELogSeverity minSeverity)
	{
		mMinSeverity = minSeverity;
	}

	void RGenericPlatformLogger::Log(ELogSeverity severity, std::string message, va_list args)
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

	ELogSeverity RGenericPlatformLogger::GetMinLogSeverity() const
	{
		return mMinSeverity;
	}
}