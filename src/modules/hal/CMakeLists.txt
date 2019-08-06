#module definition.
set(MODULE_NAME "hal")

set (SourceGroup_PublicIncludes
	"public/hal/platform.h"
	"public/hal/info.h"
	"public/hal/logger.h"
	"public/hal/file_system.h"
	"public/hal/threading.h"
	"public/hal/window.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/hal.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

#compile as static library even though it is an interface to make dependencies easier to handle.
add_library(${MODULE_NAME} STATIC ${SOURCE})

#Platform implementations.
if (WIN32)
	target_link_libraries(${MODULE_NAME} PUBLIC platform_windows)
endif()
 if(UNIX AND NOT APPLE)
	target_link_libraries(${MODULE_NAME} PUBLIC platform_linux)
endif()

target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")



