#START-FILE-LIST
set (SourceGroup_Main
	"src/main.cpp"
)
source_group("Main" FILES ${SourceGroup_Main})

set (SourceGroup_Components
	"include/moduletool/parser.h"
	"include/moduletool/project.h"
	"include/moduletool/module.h"
)
source_group("Components" FILES ${SourceGroup_Components})

set (SOURCE
	${SourceGroup_Main}
	${SourceGroup_Components}
)

add_executable(RavenProjectTool ${SOURCE} ${RAVEN_ICON_RESOURCE_PATH})
target_include_directories(RavenProjectTool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
