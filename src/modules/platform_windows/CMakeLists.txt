#module definition
set(MODULE_NAME "platform_windows")

set (SourceGroup_PublicIncludes
	"public/platform_windows/platform.h"
	"public/platform_windows/info.h"
	"public/platform_windows/window.h"
	"public/platform_windows/logger.h"
	"public/platform_windows/threading.h"
	"public/platform_windows/util/wstring.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/platform.cpp"
	"private/info.cpp"
	"private/window.cpp"
	"private/logger.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC core platform imgui)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")



