#pragma once
#include "DynamicLibrary.h"
#include <cinttypes>

#define RAVEN_PLUGIN_API_VER5ION 1

typedef struct FPluginMeta
{
	uint32_t apiVersion;
	const char* className;
	const char* pluginName;
	const char* pluginVersion;
} FPluginMeta;

#define RAVEN_PLUGIN_EXPORTS(className, pluginName, pluginVersion)  \
	extern "C"														\
	{																\
	    RVN_EXPORT FPluginMeta ##className _exports =				\
	    {															\
		  RAVEN_PLUGIN_API_VER5ION									\
	        #className,												\
	        pluginName,												\
	        pluginVersion,											\
	    };															\
	}

#if RAVEN_PLUGIN_STATIC_LINK
	#define RAVEN_PLUGIN(className, pluginName, pluginVersion)  \
	RAVEN_PLUGIN_EXPORTS(className, pluginName, pluginVersion)
#else
	#define RAVEN_PLUGIN(className, pluginName, pluginVersion)  \
	RAVEN_PLUGIN_EXPORTS(className, pluginName, pluginVersion)
#endif


