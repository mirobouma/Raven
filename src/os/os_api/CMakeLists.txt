#module definition.
set(PROJECT_NAME raven-os-api)

set (SourceGroup_PublicIncludes
	"public/os/api/native_os.h"
	"public/os/api/native_thread.h"
	"public/os/api/native_fiber.h"
	"public/os/api/native_window.h"
	"public/os/api/native_file_system.h"
	"public/os/api/native_dynamic_library.h"
	"public/os/api/monitor.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/os_api.cpp"
)

source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
    )

target_link_libraries(${PROJECT_NAME} PUBLIC raven-os raven-util raven-math)