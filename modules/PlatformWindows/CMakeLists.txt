set (SourceGroup_Platform
	"include/Platform/PlatformWindows.h"
)
source_group("Platform" FILES ${SourceGroup_Platform})

set (SOURCE
	${SourceGroup_Platform}
)

add_library(RavenPlatformWindows STATIC ${SOURCE})
set_target_properties(RavenPlatformWindows PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(RavenPlatformWindows 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(RavenPlatformWindows PUBLIC RavenPlatform)
