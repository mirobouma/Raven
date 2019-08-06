#module definition
set(MODULE_NAME "ui")

set (SourceGroup_PublicIncludes
	"public/ui/ui.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/ui.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC imgui rendering hal)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")



