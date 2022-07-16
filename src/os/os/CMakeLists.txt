set(PROJECT_NAME raven-os)

set (SourceGroup_PublicIncludes
    "public/os/file_system.h"
    "public/os/native_window.h"
    "public/os/native_thread.h"
    "public/os/native_fiber.h"
    "public/os/window_event_listener.h"
    "public/os/window_system.h"
    "public/os/os.h"
)

source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
    "private/window_system.cpp"
)

source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
    )

target_link_libraries(${PROJECT_NAME} PUBLIC ${PLATFORM_MODULE} raven-util)
