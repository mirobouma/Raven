#pragma once
#include <string>
#include <stdarg.h>

namespace Raven
{
	/*!
	\enum ELogSeverity
	\brief Enumeration for log severity of messages.
	*/
	typedef enum ELogSeverity {
		LS_Debug = 0,
		LS_Info,
		LS_Warning,
		LS_Error,
	} ELogSeverity;

	/*!
	\class IPlatformLogger
	\brief Abstract interface for platform-specific logging.
	*/
	class IPlatformLogger
	{
	protected:
		IPlatformLogger() = default; /*!< This is an abstract interface so we don't have to instantiate it. */

	public:
		/*! Log a message with a certain severity. */
		virtual void Log(ELogSeverity severity, std::string message, va_list args) = 0;

		/*! Get the minimum logging severity that should be logged. */
		virtual ELogSeverity GetMinLogSeverity() const = 0;

	};
}

