#module definition
set(MODULE_NAME "RavenPlatform")

set (SourceGroup_PublicIncludes
	"Public/Platform/Generic/GenericPlatform.h"
	"Public/Platform/Generic/GenericPlatformInfo.h"
	"Public/Platform/Generic/GenericPlatformWindow.h"
	"Public/Platform/Generic/GenericPlatformLogger.h"
	"Public/Platform/Interfaces/IPlatformWindow.h"
	"Public/Platform/Interfaces/IPlatformLogger.h"
	"Public/Platform/Interfaces/IPlatformThreading.h"
	"Public/Platform/Interfaces/IWindowMessageHandler.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/GenericPlatform.cpp"
	"Private/GenericPlatformWindow.cpp"
	"Private/GenericPlatformLogger.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

