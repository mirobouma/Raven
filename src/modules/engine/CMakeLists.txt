#module definition
set(MODULE_NAME "engine")

set (SourceGroup_PublicIncludes
	"public/engine/engine.h"
	"public/engine/globals.h"
	"public/engine/windows/renderable_window.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/engine.cpp"
	"private/globals.cpp"
	"private/windows/renderable_window.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC core hal renderer_vulkan)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")



