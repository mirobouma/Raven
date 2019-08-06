#module definition
set(MODULE_NAME "platform")

set (SourceGroup_PublicIncludes
	"public/platform/defines.h"
	"public/platform/generic/platform.h"
	"public/platform/generic/info.h"
	"public/platform/generic/file_system.h"
	"public/platform/generic/window.h"
	"public/platform/generic/logger.h"
	"public/platform/interfaces/window.h"
	"public/platform/interfaces/logger.h"
	"public/platform/interfaces/threading.h"
	"public/platform/interfaces/window_message_handler.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/generic/platform.cpp"
	"private/generic/window.cpp"
	"private/generic/logger.cpp"
	"private/generic/file_system.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC core)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

