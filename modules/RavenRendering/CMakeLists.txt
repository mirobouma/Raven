#module definition
set(MODULE_NAME "RavenRendering")

set (SourceGroup_PublicIncludes
	"include/Rendering/Window.h"
	"include/Rendering/WindowManager.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"src/WindowManager.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

#add_library(${MODULE_NAME} STATIC ${SOURCE})
#target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

