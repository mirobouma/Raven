#pragma once
#include "HAL/PlatformLogger.h"
#include <memory>
#include <string>

#define ENABLE_LOGGING

namespace Raven {
	/*!
	\class RLogManager
	\brief Logging manager, only one is allowed to exist per application.
	*/
	class RLogManager {
	private:
		/*! Initialize the log manager.
		\param minSeverity Minimum severity that is logged.
		*/
		RLogManager(ELogSeverity minSeverity);

		RLogManager(const RLogManager&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy the logging manager instance. */
		RLogManager& operator= (const RLogManager&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite the logging manager. */

	public:
		/*! Initialize the global log manager. 
		\param minSeverity Minimum severity that is logged.
		*/
		static bool Initialize(ELogSeverity minSeverity); 

		/*! Destroy the global log manager. */
		static void Destroy(); 

		/*! Destroy the global log manager. */
		static void Log(ELogSeverity severity, std::string message, ...);

	protected:
		std::unique_ptr<IPlatformLogger> mLogger; /*! Logger instance. */
	};

	extern RLogManager* GLogManager; /*!< Global reference to logging manager. */
}

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

#ifndef NDEBUG
#define RASSERT(condition) if(!(condition)) { RLOG_ERROR("Assertion failed: \"%s\"", #condition); }
#else
#define RASSERT(condition) ((void)0)
#endif


#ifdef ENABLE_LOGGING

#define RLOG(severity, message, ...) \
	RLogManager::Log(severity, message, ##__VA_ARGS__)

#define RLOG_DEBUG(message, ...) \
	RLogManager::Log(LS_Debug, message, ##__VA_ARGS__)

#define RLOG_INFO(message, ...) \
	RLogManager::Log(LS_Info, message, ##__VA_ARGS__)

#define RLOG_WARNING(message, ...) \
	RLogManager::Log(LS_Warning, message, ##__VA_ARGS__)

#define RLOG_ERROR(message, ...) \
	RLogManager::Log(LS_Error, std::string(message).append(" at line ").append(STRINGIZE(__LINE__)).append(" of file ").append(STRINGIZE(__FILE__)).append(" in function ").append(##__FUNCTION__), ##__VA_ARGS__)

#else
#define RLOG(severity, message, ...) ((void)0)
#define RLOG_DEBUG(message, ...) ((void)0)
#define RLOG_INFO(message, ...) ((void)0)
#define RLOG_WARNING(message, ...) ((void)0)
#define RLOG_ERROR(message, ...) ((void)0)
#endif



