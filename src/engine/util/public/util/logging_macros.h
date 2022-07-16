#pragma once
#include "util/logger.h"

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

#ifndef NDEBUG
#define RASSERT(condition) if(!(condition)) {  \
RLOG_ERROR("Assertion failed: \"%s\"", #condition);  \
}

#define RASSERT_MSG(condition, ...) if(!(condition)) {  \
RLOG_ERROR("Assertion failed: \"%s\"", ##__VA_ARGS__);  \
}

#else
#define RASSERT(condition) ((void)0)
#define RASSERT_MSG(condition, ...) ((void)0)
#endif


#ifndef DISABLE_LOGGING
#define RLOG(severity, message, ...) \
	Raven::LogManager::get().log(severity, message, ##__VA_ARGS__);

#define RLOG_DEBUG(message, ...) \
	Raven::LogManager::get().log(LS_Debug, message, ##__VA_ARGS__);

#define RLOG_INFO(message, ...) \
	Raven::LogManager::get().log(LS_Info, message, ##__VA_ARGS__);

#define RLOG_WARNING(message, ...) \
	Raven::LogManager::get().log(LS_Warning, message, ##__VA_ARGS__);

#define RLOG_ERROR(message, ...) \
	Raven::LogManager::get().log(LS_Error, (std::string(message).append(" at line ").append(STRINGIZE(__LINE__)).append(" of file ").append(STRINGIZE(__FILE__)).append(" in function ").append(__FUNCTION__ )).c_str(), ##__VA_ARGS__);
#else
#define RLOG(severity, message, ...) ((void)0)
#define RLOG_DEBUG(message, ...) ((void)0)
#define RLOG_INFO(message, ...) ((void)0)
#define RLOG_WARNING(message, ...) ((void)0)
#define RLOG_ERROR(message, ...) ((void)0)
#endif
