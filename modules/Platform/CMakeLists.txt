set (SourceGroup_Platform
	"include/Platform/Platform.h"
	"include/Platform/Window.h"
)
source_group("Platform" FILES ${SourceGroup_Platform})

set (SOURCE
	${SourceGroup_Platform}
)

add_library(RavenPlatform STATIC ${SOURCE})
set_target_properties(RavenPlatform PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(RavenPlatform 	
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
)