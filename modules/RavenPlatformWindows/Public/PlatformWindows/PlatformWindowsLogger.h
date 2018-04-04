#pragma once
#include "Platform/Interfaces/IPlatformLogger.h"
#include <string>

namespace Raven
{
	/*!
	\class RPlatformWindowsLogger
	\brief Windows implementation of a logger.
	*/
	class RPlatformWindowsLogger : public IPlatformLogger
	{
	public:
		/*! Initialize the windows platform logger.
		\param[in] minSeverity The minimum severity that should be logged. 
		*/
		RPlatformWindowsLogger(ELogSeverity minSeverity);

		/*! Log a message with a certain severity. 
		\param[in] severity The severity of the logged message.
		\param[in] message The message that should be logged with additional wildcard variables that have to be replaced.
		*/
		virtual void Log(ELogSeverity severity, std::string message, va_list args) override;

		/*! Get the maximum logging severity that should be logged. 
		\returns maximum logging severity of this logger. 
		*/
		virtual ELogSeverity GetMinLogSeverity() const override;

	protected:
		ELogSeverity mMinSeverity; /*!< Maximum severity that has to be logged. */
	};

	// HAL abstraction.
	typedef RPlatformWindowsLogger RPlatformLogger;
}

