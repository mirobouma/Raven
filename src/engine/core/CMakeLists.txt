set(PROJECT_NAME raven-core)

set (SourceGroup_PublicIncludes
    "public/core/engine.h"
    "public/core/errors.h"
    "public/core/path.h"
    "public/core/string.h"
    "public/core/vector.h"
    "public/core/modules/module.h"
    "public/core/modules/module_manager.h"
    "public/core/os/os.h"
    "public/core/os/file_system.h"
    "public/core/os/dynamic_library.h"
    "public/core/os/memory.h"
    "public/core/os/monitor.h"
    "public/core/os/thread.h"
    "public/core/main/main.h"
    "public/core/main/main_looper.h"
)
source_group("Public" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/core.cpp"
    "private/path.cpp"
    "private/main/main.cpp"
    "private/os/os.cpp" 
)

source_group("Source" FILES ${SourceGroup_Source})
set (SOURCES ${SourceGroup_Source} ${SourceGroup_PublicIncludes})

if(BUILD_MONOLITHIC)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")
target_link_libraries(${PROJECT_NAME} PUBLIC tinystl)
