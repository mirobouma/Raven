#START-FILE-LIST
set (SourceGroup_Main
	"src/main.cpp"
)
source_group("Main" FILES ${SourceGroup_Main})

set (SourceGroup_Components
	"include/projecttool/parser.h"
	"include/projecttool/project.h"
	"include/projecttool/module.h"
)
source_group("Components" FILES ${SourceGroup_Components})

set (SOURCE
	${SourceGroup_Main}
	${SourceGroup_Components}
)

add_executable(CtrProjectTool ${SOURCE})
target_include_directories(CtrProjectTool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
