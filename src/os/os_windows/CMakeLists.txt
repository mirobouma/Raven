set(PROJECT_NAME raven-os-windows)

set (SourceGroup_PublicIncludes
    "public/os/windows/os_windows.h"
    "public/os/windows/windows_dynamic_library.h"
    "public/os/windows/windows_file_system.h"
    "public/os/windows/windows_thread.h"
    "public/os/windows/windows_fiber.h"
    "public/os/windows/windows_window.h"
    "public/os/windows/util/wstring.h"
)

source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/os_windows.cpp"
    "private/windows_main.cpp"
    "private/windows_thread.cpp"
    "private/windows_window.cpp"
    "private/util/wstring.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
    )

target_link_libraries(${PROJECT_NAME} PUBLIC raven-os-api)
