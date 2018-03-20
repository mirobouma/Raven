#module definition
set(MODULE_NAME "RavenEngine")

set (SourceGroup_PublicIncludes
	"Public/Engine/Engine.h"
	"Public/Engine/Globals.h"
	"Public/Engine/Windows/RenderableWindow.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/Engine.cpp"
	"Private/Globals.cpp"
	"Private/Windows/RenderableWindow.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenCore RavenHAL RavenWindowSystem RavenRenderingVulkan)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")



