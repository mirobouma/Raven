#module definition
set(MODULE_NAME "RavenPlatformWindows")

set (SourceGroup_PublicIncludes
	"Public/PlatformWindows/PlatformWindows.h"
	"Public/PlatformWindows/PlatformWindowsInfo.h"
	"Public/PlatformWindows/PlatformWindowsWindow.h"
	"Public/PlatformWindows/PlatformWindowsLogger.h"
	"Public/PlatformWindows/PlatformWindowsThreading.h"
	"Public/PlatformWindows/Util/WString.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/PlatformWindows.cpp"
	"Private/PlatformWindowsInfo.cpp"
	"Private/PlatformWindowsWindow.cpp"
	"Private/PlatformWindowsLogger.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore RavenPlatform imgui)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")



