#module definition
set(MODULE_NAME "RavenPlatform")

set (SourceGroup_PublicIncludes
	"include/Platform/Platform.h"
	"include/Platform/PlatformWindow.h"
	"include/Platform/WindowSystemInterface.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"src/Platform.cpp"
	"src/WindowSystemInterface.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

