#include "ConsoleLogger.h"

namespace Raven
{
	std::string string_format(const std::string fmt, va_list ap) {
		int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
		std::string str;
		while (1) {     // Maximum two passes on a POSIX system...
			str.resize(size);
			int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
			if (n > -1 && n < size) {  // Everything worked
				str.resize(n);
				return str;
			}
			if (n > -1)  // Needed size returned
				size = n + 1;   // For null char
			else
				size *= 2;      // Guess at a larger size (OS specific)
		}
		return str;
	}

	void FConsoleLogger::Log(ELogSeverity severity, std::string message, va_list args)
	{
		if (mMessages.size() >= maxMessages)
		{
			mMessages.pop_back();
		}

		// If the severity
		if (severity < GetMinLogSeverity())
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

		std::string res = string_format(prefix + message + "\n", args);
		mMessages.push_front(res);
	}


	ELogSeverity FConsoleLogger::GetMinLogSeverity() const
	{
		return ELogSeverity::LS_Debug;
	}
}
