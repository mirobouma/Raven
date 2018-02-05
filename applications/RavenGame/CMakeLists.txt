#START-FILE-LIST
set (SourceGroup_Main
	"src/Main.cpp"
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
)

add_executable(RavenGame ${SOURCE})
target_include_directories(RavenGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
