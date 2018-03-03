#module definition
set(MODULE_NAME "RavenPlatform")

set (SourceGroup_PublicIncludes
	"Public/Platform/GenericPlatform.h"
	"Public/Platform/GenericPlatformInfo.h"
	"Public/Platform/GenericPlatformWindow.h"
	"Public/Platform/GenericPlatformApplication.h"
	"Public/Platform/WindowSystemInterface.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/GenericPlatform.cpp"
	"Private/GenericPlatformWindow.cpp"
	"Private/GenericPlatformApplication.cpp"
	"Private/WindowSystemInterface.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

