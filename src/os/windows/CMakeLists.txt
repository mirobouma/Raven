set(PROJECT_NAME raven-os-windows)

set (SourceGroup_PublicIncludes
    "public/windows/os_windows.h"
    "public/windows/windows_file_system.h"
    "public/windows/windows_thread.h"
    "public/windows/windows_memory.h"
)

source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/os_windows.cpp"
    "private/windows_main.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
    )

target_link_libraries(${PROJECT_NAME} PUBLIC raven-core)
